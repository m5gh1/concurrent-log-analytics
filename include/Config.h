#pragma once
#include <cstddef>
#include <string>

struct Config {
    std::string inputPath{"data/sample.log"};
    std::string alertOutputPath{"output/alerts.txt"};
    std::size_t workers{4};
    std::size_t queueCapacity{1024};
    std::size_t topK{5};
    std::size_t alertThreshold{3};
    std::string messageContains;
};
