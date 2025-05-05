#ifndef PROMETHEUS_HPP
#define PROMETHEUS_HPP

#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <prometheus/counter.h>
#include <prometheus/gauge.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/sysinfo.h>
#include <sys/resource.h>

using namespace prometheus;

namespace Monitoring {

    class Prometheus {
    public:
        Prometheus(const int& port = 4500)
            : exposer("0.0.0.0:" + std::to_string(port)),
              registry(std::make_shared<Registry>()),
              requestCounterFamily(BuildCounter()
                                   .Name("http_requests_total")
                                   .Help("Number of HTTP requests by type")
                                   .Register(*registry)),
              crashCounterFamily(BuildCounter()
                                   .Name("Crash_Count")
                                   .Help("Number of crashes with reason")
                                   .Register(*registry)),
              ramGaugeFamily(BuildGauge()
                               .Name("system_ram_usage_bytes")
                               .Help("System RAM usage in bytes")
                               .Register(*registry)),
              processMemoryGaugeFamily(BuildGauge()
                                         .Name("process_memory_usage_bytes")
                                         .Help("Process memory usage in bytes")
                                         .Register(*registry)) {
            exposer.RegisterCollectable(registry);
            
            // Initialize gauges
            ramGauge = &ramGaugeFamily.Add({});
            processMemoryGauge = &processMemoryGaugeFamily.Add({});
            
            // Start monitoring thread
            monitoringThread = std::thread(&Prometheus::monitorSystemMetrics, this);
        }

        ~Prometheus() {
            if (monitoringThread.joinable()) {
                monitoringThread.join();
            }
        }

        void incrementRequest(const std::string& method) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = requestCounters.find(method);
            if (it == requestCounters.end()) {
                auto& counter = requestCounterFamily.Add({{"method", method}});
                requestCounters[method] = &counter;
                counter.Increment();
            } else {
                it->second->Increment();
            }
        }

        void incrementCrash(const std::string& reason) {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = crashCounters.find(reason);
            if (it == crashCounters.end()) {
                auto& counter = crashCounterFamily.Add({{"reason", reason}});
                crashCounters[reason] = &counter;
            } else {
                it->second->Increment();
            }
        }

    private:
        void monitorSystemMetrics() {
            while (true) {
                updateSystemMetrics();
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }

        void updateSystemMetrics() {
            std::lock_guard<std::mutex> lock(mutex_);
            
            // Update RAM usage
            struct sysinfo si;
            if (sysinfo(&si) == 0) {
                ramGauge->Set(si.totalram - si.freeram);
            }

            // Update process memory usage
            struct rusage usage;
            if (getrusage(RUSAGE_SELF, &usage) == 0) {
                processMemoryGauge->Set(usage.ru_maxrss * 1024); // Convert from KB to Bytes
            }
        }

        Exposer exposer;
        std::shared_ptr<Registry> registry;
        Family<Counter>& requestCounterFamily;
        Family<Counter>& crashCounterFamily;
        Family<Gauge>& ramGaugeFamily;
        Family<Gauge>& processMemoryGaugeFamily;
        std::unordered_map<std::string, Counter*> requestCounters;
        std::unordered_map<std::string, Counter*> crashCounters;
        Gauge* ramGauge;
        Gauge* processMemoryGauge;
        std::mutex mutex_;
        std::thread monitoringThread;
    };
}

#endif