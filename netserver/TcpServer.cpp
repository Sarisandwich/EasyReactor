#include"TcpServer.h"


TcpServer::TcpServer(const std::string& ip, uint16_t port, size_t numThread):numThread_(numThread)
{
    mainloop_=std::make_unique<EventLoop>(true);
    mainloop_->set_epollTimeoutcb(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));

    acceptor_=std::make_unique<Acceptor>(mainloop_.get(), ip, port);
    acceptor_->set_newConnection_cb(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
    
    pool_=new ThreadPool(numThread_, "IO");
    for(size_t i=0;i<numThread_;++i)
    {
        subloops_.emplace_back(std::make_unique<EventLoop>(false));
        subloops_[i]->set_epollTimeoutcb(std::bind(&TcpServer::epollTimeout, this, std::placeholders::_1));
        subloops_[i]->set_timercb(std::bind(&TcpServer::removeConn, this, std::placeholders::_1));
        pool_->enqueue(std::bind(&EventLoop::run, subloops_[i].get()));
    }
}

TcpServer::~TcpServer()
{
    
    
    delete pool_;
}

void TcpServer::start()
{
    mainloop_->run();
}

void TcpServer::newConnection(std::unique_ptr<Socket> clientsock)
{
    spConnection conn=std::make_shared<Connection>(subloops_[clientsock->fd()%numThread_].get(), std::move(clientsock));
    //printf("new connection(fd=%d, ip=%s, port=%d) ok.\n", conn->fd(), conn->ip().c_str(), conn->port());
    conn->set_closecb(std::bind(&TcpServer::closeConnection, this, std::placeholders::_1));
    conn->set_errorcb(std::bind(&TcpServer::errorConnection, this, std::placeholders::_1));
    conn->set_onmessagecb(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    conn->set_sendCompletecb(std::bind(&TcpServer::sendComplete, this, std::placeholders::_1));

    {
        std::lock_guard<std::mutex> lock(mtx_);
        conns_[conn->fd()]=conn;    //conn存放到TcpServer的conns_容器。
    }
    subloops_[conn->fd()%numThread_]->newConnection(conn);  //conn存放到EventLoop的conns_容器。

    if(newConnection_cb_) newConnection_cb_(conn);
}

void TcpServer::closeConnection(spConnection conn)
{
    if(closeConnection_cb_) closeConnection_cb_(conn);
    
    {
        std::lock_guard<std::mutex> lock(mtx_);
        conns_.erase(conn->fd());
    }
}

void TcpServer::errorConnection(spConnection conn)
{
    if(errorConnection_cb_) errorConnection_cb_(conn);

    {
        std::lock_guard<std::mutex> lock(mtx_);
        conns_.erase(conn->fd());
    }
}

void TcpServer::onmessage(spConnection conn, std::string& message)
{
    if(onmessage_cb_) onmessage_cb_(conn, message);
}

void TcpServer::sendComplete(spConnection conn)
{
    if(sendComplete_cb_) sendComplete_cb_(conn);
}

void TcpServer::epollTimeout(EventLoop* loop)
{
    if(epollTimeout_cb_) epollTimeout_cb_(loop);
}

void TcpServer::removeConn(int fd)
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        conns_.erase(fd);
    }
}

void TcpServer::set_newConnectioncb(std::function<void(spConnection)> func)
{
    newConnection_cb_=func;
}

void TcpServer::set_closeConnectioncb(std::function<void(spConnection)> func)
{
    closeConnection_cb_=func;
}

void TcpServer::set_errorConnectioncb(std::function<void(spConnection)> func)
{
    errorConnection_cb_=func;
}

void TcpServer::set_onmessagecb(std::function<void(spConnection,std::string& message)> func)
{
    onmessage_cb_=func;
}

void TcpServer::set_sendCompletecb(std::function<void(spConnection)> func)
{
    sendComplete_cb_=func;
}

void TcpServer::set_epollTimeoutcb(std::function<void(EventLoop*)> func)
{
    epollTimeout_cb_=func;
}