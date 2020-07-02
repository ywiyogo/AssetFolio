// Author: YWiyogo
// Descr.: MessageQueue implementation for the communication between the
// wxThread and the API updater async task.

#ifndef MSGQUEUE_H_
#define MSGQUEUE_H_

#include <condition_variable>
#include <deque>
#include <iostream>
#include <mutex>
using namespace std;

// Due to the rvalue reference for the send function, this struct is declared
class UpdateData
{
  public:
    // Asset ID
    string _id;
    float _curr_price;
    float _curr_value;
    float _diff;
    float _diff_in_percent;
    float _return;
    float _return_in_percent;
    float _profit_loss;

    UpdateData(){};
    // Constructor
    UpdateData(string id, float currprice, float currvalue, float diff,
               float diff_in_percent, float ret, float returninpercent, float profitloss)
    {
        _id = id;
        _curr_price = currprice;
        _curr_value = currvalue;
        _diff = diff;
        _diff_in_percent = diff_in_percent;
        _return = ret;
        _return_in_percent = returninpercent;
        _profit_loss = profitloss;
    };
    UpdateData(const UpdateData& source) // 2 : copy constructor
    {
        _id = source._id;
        _curr_price = source._curr_price;
        _curr_value = source._curr_value;
        _diff = source._diff;
        _diff_in_percent = source._diff_in_percent;
        _return = source._return;
        _return_in_percent = source._return_in_percent;
        _profit_loss = source._profit_loss;
        // std::cout << "COPYING content of instance " << source._id << " "
        //           << &source << " to instance " << this << std::endl;
    }
    // 3. copy assignment operator
    UpdateData& operator=(const UpdateData& source)
    {
        std::cout << "ASSIGNING content of instance " << source._id << " "
                  << &source << " to instance " << this << std::endl;
        if (this == &source)
            return *this;

        _id = source._id;
        _curr_price = source._curr_price;
        _curr_value = source._curr_value;
        _diff = source._diff;
        _diff_in_percent = source._diff_in_percent;
        _return = source._return;
        _return_in_percent = source._return_in_percent;
        _profit_loss = source._profit_loss;
        return *this;
    }

    UpdateData(UpdateData&& source) // 4 : move constructor
    {
        // std::cout << "MOVING (c’tor) instance " << source._id << " " << &source
        //           << " to instance " << this << std::endl;
        _id = source._id;
        _curr_price = source._curr_price;
        _curr_value = source._curr_value;
        _diff = source._diff;
        _diff_in_percent = source._diff_in_percent;
        _return = source._return;
        _return_in_percent = source._return_in_percent;
        _profit_loss = source._profit_loss;
    }

    // 5. Move constructor
    UpdateData& operator=(UpdateData&& source) noexcept // move assignment
    {
        // std::cout << "MOVING UpdateData assign instance " << source._id << " "
        //           << &source << " to instance " << this << std::endl;
        if (this == &source)
            return *this;
        _id = source._id;
        _curr_price = source._curr_price;
        _curr_value = source._curr_value;
        _diff = source._diff;
        _diff_in_percent = source._diff_in_percent;
        _return = source._return;
        _return_in_percent = source._return_in_percent;
        _profit_loss = source._profit_loss;
        return *this;
    }

    ~UpdateData(){};

  private:
};

//-----------------------------

template <class T> class MsgQueue
{
  public:
    void send(unique_ptr<T> msg);

    unique_ptr<T> waitForUpdate(); // a.k.a receive

    // Get queue size for debugging
    int size();

    // Reseting the queue
    void clear();

    ~MsgQueue(){};

  private:
    deque<unique_ptr<T>> _queue;
    condition_variable _condition;
    mutex _mutex;
};

// Implementation in the same header file due to the error:
// undefined reference to `MsgQueue<UpdateData>::waitForUpdate()'
template <typename T> unique_ptr<T> MsgQueue<T>::waitForUpdate()
{
    // cout << "MsqQueue: wait for update. Queue size: " << _queue.size() <<
    // endl
    //      << flush;
    unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this] { return !_queue.empty(); });
    // Retrieve the element from queue, and remove the first vector element from
    // queue
    unique_ptr<T> msg = move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T> void MsgQueue<T>::send(unique_ptr<T> msg)
{
    lock_guard<std::mutex> uLock(_mutex);
    // cout << "    MsqQueue: update data is sent, id: " << msg->_id << endl
    //      << flush;
    // add vector to queue
    _queue.emplace_back(move(msg));
    _condition
        .notify_one(); // notify client after pushing new msg into the queue
}

template <typename T> int MsgQueue<T>::size() { return _queue.size(); }

template <typename T> void MsgQueue<T>::clear() { return _queue.clear(); }
#endif