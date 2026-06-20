#include <fstream>
#include <vector>
#include <iostream>
#include <cstdint>

const size_t SECTOR_SIZE = 512;
const size_t TOTAL_SECTORS = 2880;

std::vector<uint8_t> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error: Cannot open " << filename << std::endl;
        return {};
    }
    
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    return data;
}

int main() {
    std::vector<uint8_t> image(TOTAL_SECTORS * SECTOR_SIZE, 0);
    
    // Загружаем загрузчик
    auto bootloader = readFile("build/bootloader.bin");
    if (bootloader.empty()) {
        std::cerr << "Error: build/bootloader.bin not found" << std::endl;
        return 1;
    }
    
    // Копируем загрузчик в первый сектор
    if (bootloader.size() <= SECTOR_SIZE) {
        std::copy(bootloader.begin(), bootloader.end(), image.begin());
    } else {
        std::cerr << "Bootloader too large: " << bootloader.size() << " bytes" << std::endl;
        return 1;
    }
    
    // Загружаем ядро
    auto kernel = readFile("build/kernel.bin");
    if (kernel.empty()) {
        std::cerr << "Error: build/kernel.bin not found" << std::endl;
        return 1;
    }
    
    // Копируем ядро в сектора 2+
    if (kernel.size() <= (TOTAL_SECTORS - 1) * SECTOR_SIZE) {
        std::copy(kernel.begin(), kernel.end(), image.begin() + SECTOR_SIZE);
    } else {
        std::cerr << "Kernel too large: " << kernel.size() << " bytes" << std::endl;
        return 1;
    }
    
    // Проверяем сигнатуру
    if (image[510] != 0x55 || image[511] != 0xAA) {
        image[510] = 0x55;
        image[511] = 0xAA;
    }
    
    // Записываем образ
    std::ofstream out("build/os.img", std::ios::binary);
    if (!out) {
        std::cerr << "Error: Cannot create build/os.img" << std::endl;
        return 1;
    }
    
    out.write(reinterpret_cast<char*>(image.data()), image.size());
    out.close();
    
    std::cout << "Created bootable disk image" << std::endl;
    std::cout << "Bootloader: " << bootloader.size() << " bytes" << std::endl;
    std::cout << "Kernel: " << kernel.size() << " bytes" << std::endl;
    
    return 0;
}