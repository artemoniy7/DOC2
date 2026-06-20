// src/build_image.cpp (проверочная версия)
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

std::vector<uint8_t> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error: Cannot open " << filename << std::endl;
        return {};
    }
    size_t size = file.tellg();
    file.seekg(0);
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    std::cout << "Read " << filename << ": " << size << " bytes" << std::endl;
    return data;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <output.img>" << std::endl;
        return 1;
    }

    // Чтение загрузчика
    auto bootloader = readFile("build/boot.bin");
    if (bootloader.size() != 512) {
        std::cerr << "Error: boot.bin must be 512 bytes" << std::endl;
        return 1;
    }

    // Чтение ядра
    auto kernel = readFile("build/kernel.bin");
    if (kernel.empty()) {
        std::cerr << "Error: kernel.bin is empty" << std::endl;
        return 1;
    }

    // Создание образа
    std::ofstream output(argv[1], std::ios::binary);
    if (!output) {
        std::cerr << "Error: Cannot create " << argv[1] << std::endl;
        return 1;
    }

    // Запись загрузчика (сектор 1)
    output.write(reinterpret_cast<const char*>(bootloader.data()), bootloader.size());

    // Запись ядра (секторы 2+)
    output.write(reinterpret_cast<const char*>(kernel.data()), kernel.size());

    // Заполнение до кратности 512 байт
    size_t total = bootloader.size() + kernel.size();
    size_t pad = (512 - (total % 512)) % 512;
    if (pad > 0) {
        std::vector<uint8_t> zeros(pad, 0);
        output.write(reinterpret_cast<const char*>(zeros.data()), zeros.size());
    }

    std::cout << "Created " << argv[1] << ": " 
              << bootloader.size() + kernel.size() + pad << " bytes" << std::endl;
    std::cout << "Kernel sectors: " << (kernel.size() + 511) / 512 << std::endl;

    return 0;
}