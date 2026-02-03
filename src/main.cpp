#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "io/ConfigReader.h"
#include "io/Logger.h"

#ifdef _OPENMP
#include <omp.h>
#endif

void printUsage(const char* programName) {
    std::cout << "CFD Combustion Engine Simulation System\n";
    std::cout << "Usage: " << programName << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --mode=<mode>       Operation mode: mesh, solve, postprocess\n";
    std::cout << "  --config=<file>     Configuration file (JSON format)\n";
    std::cout << "  --threads=<n>       Number of OpenMP threads (default: auto)\n";
    std::cout << "  --log=<file>        Optional log file path\n";
    std::cout << "  --log-level=<lvl>   Log level: trace, debug, info, warn, error\n";
    std::cout << "  --help              Display this help message\n";
    std::cout << "  --version           Display version information\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " --mode=mesh --config=config.json\n";
    std::cout << "  " << programName << " --mode=solve --config=config.json --threads=8\n";
}

void printVersion() {
    std::cout << "CFD Combustion Engine v1.0.0\n";
    std::cout << "Built with C++17\n";
    std::cout << "Copyright (c) 2024\n";
}

struct CommandLineArgs {
    std::string mode;
    std::string configFile;
    std::string logFile;
    std::string logLevel = "info";
    int numThreads = -1;  // -1 means auto-detect
    bool showHelp = false;
    bool showVersion = false;
};

CommandLineArgs parseCommandLine(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } else if (arg == "--version" || arg == "-v") {
            args.showVersion = true;
        } else if (arg.find("--mode=") == 0) {
            args.mode = arg.substr(7);
        } else if (arg.find("--config=") == 0) {
            args.configFile = arg.substr(9);
        } else if (arg.find("--threads=") == 0) {
            args.numThreads = std::stoi(arg.substr(10));
        } else if (arg.find("--log=") == 0) {
            args.logFile = arg.substr(6);
        } else if (arg.find("--log-level=") == 0) {
            args.logLevel = arg.substr(12);
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            args.showHelp = true;
        }
    }
    
    return args;
}

int runMeshGeneration(const std::string& configFile) {
    cfd::Logger::instance().info("=== Mesh Generation Mode ===");
    cfd::Logger::instance().info("Loading config: " + configFile);
    cfd::ConfigReader reader = cfd::ConfigReader::fromFile(configFile);
    for (const auto& warning : reader.warnings()) {
        cfd::Logger::instance().warn(warning);
    }
    if (!reader.isValid()) {
        for (const auto& error : reader.errors()) {
            cfd::Logger::instance().error(error);
        }
        return 1;
    }
    cfd::Logger::instance().info("Config summary:\n" + reader.summarize());
    cfd::Logger::instance().info("Mesh generation pipeline not yet implemented.");
    return 0;
}

int runSimulation(const std::string& configFile, int numThreads) {
    cfd::Logger::instance().info("=== Simulation Mode ===");
    cfd::Logger::instance().info("Loading config: " + configFile);
    cfd::ConfigReader reader = cfd::ConfigReader::fromFile(configFile);
    for (const auto& warning : reader.warnings()) {
        cfd::Logger::instance().warn(warning);
    }
    if (!reader.isValid()) {
        for (const auto& error : reader.errors()) {
            cfd::Logger::instance().error(error);
        }
        return 1;
    }
    cfd::Logger::instance().info("Config summary:\n" + reader.summarize());
    cfd::Logger::instance().info("Threads: " + (numThreads > 0 ? std::to_string(numThreads) : "auto"));
    cfd::Logger::instance().info("Simulation pipeline not yet implemented.");
    return 0;
}

int runPostProcessing(const std::string& configFile) {
    cfd::Logger::instance().info("=== Post-Processing Mode ===");
    cfd::Logger::instance().info("Loading config: " + configFile);
    cfd::ConfigReader reader = cfd::ConfigReader::fromFile(configFile);
    for (const auto& warning : reader.warnings()) {
        cfd::Logger::instance().warn(warning);
    }
    if (!reader.isValid()) {
        for (const auto& error : reader.errors()) {
            cfd::Logger::instance().error(error);
        }
        return 1;
    }
    cfd::Logger::instance().info("Config summary:\n" + reader.summarize());
    cfd::Logger::instance().info("Post-processing pipeline not yet implemented.");
    return 0;
}

int main(int argc, char* argv[]) {
    CommandLineArgs args = parseCommandLine(argc, argv);
    
    if (args.showHelp) {
        printUsage(argv[0]);
        return 0;
    }
    
    if (args.showVersion) {
        printVersion();
        return 0;
    }
    
    // Validate arguments
    if (args.mode.empty()) {
        std::cerr << "Error: --mode is required\n";
        printUsage(argv[0]);
        return 1;
    }
    
    if (args.configFile.empty()) {
        std::cerr << "Error: --config is required\n";
        printUsage(argv[0]);
        return 1;
    }
    
    if (!args.logFile.empty()) {
        cfd::Logger::instance().setLogFile(args.logFile);
    }

    if (args.logLevel == "trace") {
        cfd::Logger::instance().setLevel(cfd::LogLevel::Trace);
    } else if (args.logLevel == "debug") {
        cfd::Logger::instance().setLevel(cfd::LogLevel::Debug);
    } else if (args.logLevel == "info") {
        cfd::Logger::instance().setLevel(cfd::LogLevel::Info);
    } else if (args.logLevel == "warn") {
        cfd::Logger::instance().setLevel(cfd::LogLevel::Warn);
    } else if (args.logLevel == "error") {
        cfd::Logger::instance().setLevel(cfd::LogLevel::Error);
    } else {
        std::cerr << "Unknown log level: " << args.logLevel << "\n";
        return 1;
    }

    cfd::Logger::instance().debug("Logger initialized.");

    // Set OpenMP threads if specified
    if (args.numThreads > 0) {
        #ifdef _OPENMP
        omp_set_num_threads(args.numThreads);
        #endif
    }
    
    // Execute based on mode
    try {
        if (args.mode == "mesh") {
            return runMeshGeneration(args.configFile);
        } else if (args.mode == "solve") {
            return runSimulation(args.configFile, args.numThreads);
        } else if (args.mode == "postprocess") {
            return runPostProcessing(args.configFile);
        } else {
            std::cerr << "Error: Unknown mode '" << args.mode << "'\n";
            std::cerr << "Valid modes: mesh, solve, postprocess\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
