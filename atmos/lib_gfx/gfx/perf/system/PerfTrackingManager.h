
#pragma once

struct perf_test_config;
class performance_instrument_scheduler;
class texture_handle;

// maintains a record of a given piece of scalar performance data(fps, # tris drawn, texture memory, etc.)
// and provides convenience functions for getting the average, etc.
class scalar_performance_data
{
public:
    struct value_record
    {
        f32_t value;
        double record_time;
    };
    
public:
    scalar_performance_data()
    {
    }
    
    scalar_performance_data(f32_t min_value, f32_t max_value, f32_t baseline_value)
    : _min_value(min_value)
    , _max_value(max_value)
    , _baseline_value(baseline_value)
    {
    }
    
    void update(const f32_t value, const double time)
    {
        value_record record;
        record.value      = value;
        record.record_time = time;
        
        _value_records.push_back(record);
        
        _last_update_time = time;
        
        _running_total += value;
        
        if (_value_records.size() > s_max_history_count)
        {
            _value_records.pop_front();
            _running_total -= _value_records.front().value;
        }
    }
    
    f32_t get_value() const
    {
        if (!_value_records.empty())
            return _value_records.back().value;
        else
            return _baseline_value;
    }
    
    const std::deque<value_record>& get_history() const
    {
        return _value_records;
    }
    
    f32_t get_min_value() const
    {
        return _min_value;
    }
    
    f32_t get_max_value() const
    {
        return _max_value;
    }
    
    f32_t get_baseline_value() const
    {
        return _baseline_value;
    }
    
    f32_t get_average() const
    {
        return _running_total / (f32_t)(_value_records.size());
    }
    
    void reset()
    {
        _value_records.clear();
    }
    
    uint32_t get_max_history_size() const
    {
        return s_max_history_count;
    }
    
private:
    std::deque<value_record> _value_records;
    f32_t _min_value = 0.0f;
    f32_t _max_value = 0.0f;
    f32_t _baseline_value = 0.0f;
    f32_t _running_total = 0.0f;
    double _last_update_time = 0.0;
    static const uint32_t s_max_history_count = 30u;
};

class perf_tracking_manager
{
public:
    
    struct profile_config
    {
        int delay = 0;
        int frames_per_test = 30;
        int frames_to_filter = 10;
    };
    
    void make();
    virtual void destroy() override;
    
    const scalar_performance_data& get_fps() const;
    f32_t get_fps_average();

    void                         record_frame_time(const f64_t unthrottled_dt);
    
    const std::shared_ptr<performance_instrument_scheduler>& scheduler() const;
        
    void batch_system_profile(const profile_config&);
    
    void config(const perf_test_config&);
    
    const perf_test_config& config() const;
    
private:
    
    scalar_performance_data _fps_data;
    std::shared_ptr<perf_test_config> _config;
    std::shared_ptr<performance_instrument_scheduler> _scheduler;
    std::map<int, std::shared_ptr<texture_handle>> _replacement_textures;
};

