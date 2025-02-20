#include <iostream>
#include <fstream>
#include <vector>
#include <type_traits>
#include <cstring>
#include <stdexcept>

/**
 * @brief Writes non-pointer fundamental types to a binary file.
 * 
 * This function takes a set of non-pointer fundamental types and writes them to the specified
 * output file in binary format. It uses a fold expression to expand and write each argument
 * in the provided parameter pack.
 * 
 * @param file A reference to an `std::ofstream` object where the data will be written.
 * @param args The non-pointer fundamental types to be written.
 * 
 * @return A reference to the `std::ofstream` after writing the data.
 * 
 * @note This function will throw a `std::runtime_error` if the file stream is not open.
 * 
 * @example
 * std::ofstream file("output.bin", std::ios::binary | std::ios::out);
 * int x = 42;
 * float y = 3.14f;
 * writeToFile(file, x, y);  // Writes `x` and `y` to the file in binary format
 */
template <typename... Args>
typename std::enable_if<(std::is_fundamental_v<Args> && ... && !std::is_pointer_v<Args>), std::ofstream&>::type
writeToFile(std::ofstream& file, Args... args) {
    if (!file) {
        throw std::runtime_error("File stream is not open");
    }

    // Expand the parameter pack and write each argument
    ((file.write(reinterpret_cast<const char*>(&args), sizeof(args))), ...);

    return file;
}

/**
 * @brief Writes non-pointer fundamental types to raw memory.
 * 
 * This function takes a set of non-pointer fundamental types and writes them to the provided
 * memory location. It uses a fold expression to expand and copy each argument in the parameter pack.
 * 
 * @param buffer A pointer to the raw memory where the data will be written.
 * @param args The non-pointer fundamental types to be written.
 * 
 * @return A pointer to the next memory location after the written data.
 * 
 * @note This function will throw a `std::runtime_error` if the buffer is null.
 * 
 * @example
 * char buffer[100];
 * int x = 42;
 * float y = 3.14f;
 * writeToRawMemory(buffer, x, y);  // Writes `x` and `y` to the buffer
 */
template <typename... Args>
typename std::enable_if<(std::is_fundamental_v<Args> && ... && !std::is_pointer_v<Args>), char*>::type
writeToRawMemory(char* buffer, Args... args) {
    if (!buffer) {
        throw std::runtime_error("Null buffer pointer");
    }

    char* ptr = buffer;
    ((std::memcpy(ptr, &args, sizeof(args)), ptr += sizeof(args)), ...);

    return ptr;
}

/**
 * @brief Writes non-pointer fundamental types to a pre-allocated buffer.
 * 
 * This function takes a set of non-pointer fundamental types and writes them to the provided
 * pre-allocated buffer starting at the specified offset. It returns the new offset after writing
 * the data, allowing further writes to continue from that position.
 * 
 * @param buffer A reference to an `std::vector<char>` where the data will be written.
 * @param offset The current offset in the buffer where the data will be written.
 * @param args The non-pointer fundamental types to be written.
 * 
 * @return The new offset after the data has been written to the buffer.
 * 
 * @throws std::runtime_error If there is insufficient space in the buffer.
 * 
 * @example
 * std::vector<char> buffer(100);
 * size_t offset = 0;
 * int x = 42;
 * float y = 3.14f;
 * offset = writeToBuffer(buffer, offset, x, y);  // Writes `x` and `y` to the buffer and updates the offset
 */
template <typename... Args>
typename std::enable_if<(std::is_fundamental_v<Args> && ... && !std::is_pointer_v<Args>), size_t>::type
writeToBuffer(std::vector<char>& buffer, size_t offset, Args... args) {
    size_t requiredSize = (sizeof(args) + ...);

    if (offset + requiredSize > buffer.size()) {
        throw std::runtime_error("Buffer too small for data");
    }

    char* ptr = buffer.data() + offset;
    writeToRawMemory(ptr, args...);

    return offset + requiredSize;  // Return the new offset after writing the data
}
