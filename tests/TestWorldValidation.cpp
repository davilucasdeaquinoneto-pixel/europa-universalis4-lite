#include "world/World.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace {

bool writeFile(const std::filesystem::path& path, const std::string& content) {
    std::ofstream file(path);
    file << content;
    return static_cast<bool>(file);
}

bool makeScenario(const std::filesystem::path& dir,
                  const std::string& countries,
                  const std::string& provinces = "id,name,owner,region_id,development,terrain\n1,Capital,TST,1,5,plains\n") {
    std::filesystem::create_directories(dir);
    return writeFile(dir / "countries.csv", countries) &&
           writeFile(dir / "regions.csv", "id,name\n1,Test Region\n") &&
           writeFile(dir / "provinces.csv", provinces) &&
           writeFile(dir / "adjacencies.csv", "province_a,province_b\n");
}

bool expectLoad(const std::filesystem::path& dir, bool expected, const std::string& label) {
    eulite::World world;
    std::string error;
    const bool loaded = world.load(dir.string(), error);
    if (loaded != expected) {
        std::cerr << "FALHOU: " << label << " (loaded=" << loaded
                  << ", error=" << error << ")\n";
        return false;
    }
    return true;
}

} // namespace

int main() {
    const auto root = std::filesystem::temp_directory_path() / "europa-lite-loader-tests";
    std::error_code ec;
    std::filesystem::remove_all(root, ec);

    bool passed = true;

    const auto valid = root / "valid";
    passed &= makeScenario(valid,
        "tag,name,treasury,manpower,tax_modifier\n"
        "TST,\"The \"\"Free\"\" State, North\",10,1000,1.0\n");
    {
        eulite::World world;
        std::string error;
        passed &= world.load(valid.string(), error);
        const auto* country = world.findCountry("TST");
        if (!country || country->name != "The \"Free\" State, North") {
            std::cerr << "FALHOU: campos CSV com vírgula e aspas escapadas\n";
            passed = false;
        }
    }

    const auto nan = root / "nan";
    passed &= makeScenario(nan,
        "tag,name,treasury,manpower,tax_modifier\nTST,Test,NaN,1000,1.0\n");
    passed &= expectLoad(nan, false, "rejeitar NaN no tesouro");

    const auto infinity = root / "infinity";
    passed &= makeScenario(infinity,
        "tag,name,treasury,manpower,tax_modifier\nTST,Test,10,1000,inf\n");
    passed &= expectLoad(infinity, false, "rejeitar infinito no modificador");

    const auto malformed = root / "malformed";
    passed &= makeScenario(malformed,
        "tag,name,treasury,manpower,tax_modifier\nTST,\"Nome sem fechar,10,1000,1.0\n");
    passed &= expectLoad(malformed, false, "rejeitar aspas não fechadas");

    const auto badHeader = root / "bad-header";
    passed &= makeScenario(badHeader,
        "tag,name,money,manpower,tax_modifier\nTST,Test,10,1000,1.0\n");
    passed &= expectLoad(badHeader, false, "rejeitar cabeçalho incorreto");

    const auto badDevelopment = root / "bad-development";
    passed &= makeScenario(badDevelopment,
        "tag,name,treasury,manpower,tax_modifier\nTST,Test,10,1000,1.0\n",
        "id,name,owner,region_id,development,terrain\n1,Capital,TST,1,-inf,plains\n");
    passed &= expectLoad(badDevelopment, false, "rejeitar desenvolvimento infinito");

    std::filesystem::remove_all(root, ec);
    if (!passed) return 1;
    std::cout << "Todos os testes de validação CSV passaram.\n";
    return 0;
}
