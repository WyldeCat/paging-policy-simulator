#include "page_policy.hpp"

#include <stdio.h>

CLOCKPRO::CLOCKPRO(size_t mem_size, size_t hot_size_ratio) : PagePolicy(mem_size), hot_size_ratio_(hot_size_ratio)
{
    hot_size_ = mem_size_ * hot_size_ratio_;
    cold_size_ = mem_size_ - hot_size_;

    hand_hot_ = 0;
    hand_cold_ = 0;
    hand_test_ = 0;

    hot_count_ = 0;
    cold_count_ = 0;
    test_count_ = 0;
}

bool CLOCKPRO::add_memtrace_(const Record &record)
{
    long vpn = record.addr << 14;

    // check if the page is in the data

    // case 0: the page is in the cache
    if (data_cache_.find(vpn) != data_cache_.end())
    {
        if (data_cache_[vpn] != -1)
        {
            if (cold_count_ < max_size_)
            {
                cold_count_++;
            }

            data_cache_[vpn] = 0;

            // test
            delete_meta_data_(new std::pair<long, char>(vpn, 2));
            test_count_--;

            add_meta_data_(new std::pair<long, char>(vpn, 1));
            hot_count_++;
            stat_set_miss_++;
        }
        else
        {
            data_cache_[key] = 1;
            stat_set_hits_++;
        }
    }

    // case 1: the page isn't in the cache
    else
    {
        data_cache_[vpn] = 0;
        add_meta_data_(new std::pair<long, char>(vpn, 0));
        cold_count_++;
        stat_set_miss_++;
    }
}

void CLOCKPRO::delete_meta_data_(pair<long, char> meta_datum)
{
    size_t meta_index = meta_data_.find(meta_datum);
    meta_data_.erase(meta_datum);
    int max_position = meta_data_.size() - 1;

    // if the hot hand
    if (hand_hot_ >= meta_index)
    {
        hand_hot_--;
        if (hand_hot_ < 0)
        {
            hand_hot_ = max_position;
        }
    }

    if (hand_cold_ >= meta_index)
    {
        hand_cold_--;
        if (hand_cold_ < 0)
        {
            hand_cold_ = max_position;
        }
    }

    if (hand_test_ >= meta_index)
    {
        hand_test_--;
        if (hand_test_ < 0)
        {
            hand_test_ = max_position;
        }
    }
}

void CLOCKPRO::add_meta_data_(pair<long, char> meta_datum)
{
    evict_pages_();
    meta_data_.insert(hand_hot_, meta_datum);
    int max_position = meta_data_.size();

    if (hand_cold_ > hand_hot_)
    {
        hand_cold_++;
        if (hand_cold_ > max_position)
        {
            hand_cold_ = 0;
        }
    }

    if (hand_test_ > hand_hot)
    {
        hand_test_++;
        if (hand_test_ > max_position)
        {
            hand_test_ = 0;
        }
    }

    hand_hot_++;
    if (hand_hot_ > max_position)
    {
        hand_hot_ = 0;
    }
}

void CLOCKPRO::evict_pages_()
{
    while (max_size_ <= cold_count_ + hot_count_)
    {
        cold_action();
    }
}

void CLOCKPRO::cold_action_(long current_page_num)
{
    std::pair<long, char> meta_datum = meta_data[hand_cold_];

    // if cold
    if (meta_datum.second == 0)
    {

        if ()
        { // TODO: if reference bit is set
            // hot
            meta_datum.second = 1;
            // TODO: set the reference bit to 0
            hot_count_++;
            cold_count_--;
        }

        // ref bit is 0
        else
        {
            // change the type to test
            meta_datum.second = 2;
            cold_count--;
            test_count++;

            while (max_size_ < test_count_)
            {
                test_action_();
            }
        }

        hand_cold_++;
        if (hand_cold_ >= meta_data_.size())
        {
            hand_cold_ = 0;
        }

        while (max_size_ - max_cold_ < hot_count_)
        {
            hot_action_();
        }
    }
    else
    {
    }

    // case 0
    //00 if the reference bit of the cold page is unset,
    //then replace it with a free space

    //replace page -> remain as a non-resident
    // until it  runs out of its test period (if it's in it)

    // if not, just move out of the clock

    // case 1
    // reference bit 1

    // and it's in the test period
    // change to the hot page
    // call the hot page action

    // not in the test period
    // do nothing

    // both cases: restet the reference bit,
    // move it to the head

    // hand keeps moving until it meet a cold page to replace
    // stops at the next resident cold page
}

void CLOCKPRO::hot_action(long current_page_num)
{
    if (hand_hot_ == hand_test_)
    {
        test_action_();
    }

    std::pair<long, char> meta_datum = meta_data_[hand_hot_];

    // if hot
    if (meta_datum.second = 1)
    {
        // set the reference bit
        if (data_cache_[meta_datum.first] == 1)
        {
            data_cache_[meta_datum.first] = 0;
        }

        else
        {
            meta_datum.second = 0;
            data_cache_[meta_datum.first] = 0;
            hot_count_--;
            cold_count_++;
        }
        hand_hot_++;
        if (hand_hot_ >= meta_data_.size())
        {
            hand_hot_ = 0;
        }
    }

    // case 0 reference bit 0
    //change the status, move forward

    // case 1 bit 1
    // reset the bit, keep it as a hot page
    // move forward, do the same on the hot pages

    // until it meets a hot page with 0 bit
    // the page is changed to a cold page

    //when the hot_hand meets a cold pahe
    // it terminates its test period
}

void CLOCKPRO::test_action()
{
    if(hand_test_ == hand_cold_){
        cold_action_();
    }

    std::pair<long, size_t> meta_datum = meta_data_[hand_test_];
    //if test
    if(meta_datum.second == 2){
        data_cache_.erase(meta_datum.first);
        delete_meta_data_(meta_datum);

        test_count_--;
        if(max_cold_ > 1){
            max_cold_--;
        }
    }

    hand_test_++;
    if(hand_test_ >= meta_data_.size()){
        hand_test_ = 0;
    }
}

bool CLOCKPRO::remove_page_(long page_number)
{
}