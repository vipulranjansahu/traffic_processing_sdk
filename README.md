# Traffic Processing SDK

The Traffic Processing SDK is designed to facilitate the handling of web server responses by logging the data and subsequently sending it to a Kafka producer. The project utilizes the librdkafka library for Kafka integration and libcpr for making HTTP requests to the server. It is as a CMake project.

## Primary Functionalities

### Log Web Server Responses
The SDK captures and logs responses received from a web server.

### Send Data to Kafka Producer
The logged responses are sent to a Kafka producer for further processing and distribution.

## Key Components

- **traffic_processing_sdk.cpp:**
  - Main implementation of the Traffic Processing SDK.
  - Functionality for creating a Kafka producer, handling signals for graceful termination, and making HTTP requests using the libcpr library.

- **CMakeLists.txt:**
  - CMake configuration file that sets up the project, specifies the C++ standard, and manages dependencies.

- **vcpkg Integration:**
  - Utilizes vcpkg to install and manage the librdkafka library.
  - Configures the build to include necessary DLLs for Windows platforms.

- **Integration with Kafka (librdkafka):**
  - Uses librdkafka to interact with Kafka, providing the necessary configurations and linking libraries.

- **Integration with HTTP Requests (libcpr):**
  - Incorporates the libcpr library for making HTTP requests to the web server.

## Future Enhancements

The project aims to be packaged as an SDK to facilitate plug-and-play integration into other applications. This modular approach allows developers to easily include traffic processing capabilities in their projects without extensive modifications.

## Testing Documentation

Follow this link for detailed testing : https://docs.google.com/document/d/1PCtji7W9pUrobwRyat7FAOHtmiQ_1CTNT_ss1Tk8_ZA/edit?usp=sharing

## Setup

### Prerequisites
Ensure you have CMake installed on your system

### Cloning

```bash
$ git clone https://github.com/vipulranjansahu/traffic_processing_sdk.git
$ cd traffic_processing_sdk
```

### Dependencies Installation

#### Installing librdkafka using vcpkg

1. Install vcpkg (This is used because CMake build for librdkafka is unsupported):
   Form inside the traffic_processing_sdk directory run:
    ```bash
    $ git clone https://github.com/Microsoft/vcpkg.git
    $ cd vcpkg
    $ ./bootstrap-vcpkg.sh
    $ ./vcpkg integrate install
    ```

2. Install librdkafka:

    ```bash
    $ ./vcpkg install librdkafka
    ```

### Building
Form inside the traffic_processing_sdk directory run:
```bash
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
$ .\Debug\traffic_processor.exe <brokers> <topic>
```
- Note : If face any issue try rebuilding
Type URL and hit enter to make an HTTP request and produce a message to Kafka.