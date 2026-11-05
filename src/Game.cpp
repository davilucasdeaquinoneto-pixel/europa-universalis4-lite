#include "simulation/Game.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

namespace eulite {

bool Game::start(const std::string& dataDirectory, std::string& error) {
    if (!world_.load(dataDirectory, error)) return false;
    started_ = true;
    return true;
}

void Game::advanceMonths(int count) {
    if (!started_ || count < 0) return;

    for (int i = 0; i < count; ++i) {
        processMonthlyEconomy();
        date_.advanceMonth();
    }
}

void Game::processMonthlyEconomy() {
    // Temporary baseline economy: owned development generates monthly tax.
    // Trade, production, expenses and modifiers will be separate systems later.
    for (auto& country : world_.countries()) {
        const double development = world_.developmentOf(country.tag);
        const double monthlyTax = development * 0.10 * country.taxModifier;
        country.treasury += monthlyTax;
    }
}

void Game::printStatus() const {
    std::cout << "\n=== EUROPA UNIVERSALIS LITE ===\n";
    std::cout << "Data: " << date_.toString() << "\n";
    std::cout << "Países: " << world_.countries().size()
              << " | Províncias: " << world_.provinces().size() << "\n\n";

    std::cout << std::left << std::setw(8) << "TAG"
              << std::setw(24) << "PAÍS"
              << std::right << std::setw(10) << "PROV."
              << std::setw(14) << "DESENV."
              << std::setw(14) << "TESOURO" << "\n";

    for (const auto& country : world_.countries()) {
        std::cout << std::left << std::setw(8) << country.tag
                  << std::setw(24) << country.name
                  << std::right << std::setw(10) << world_.countOwnedProvinces(country.tag)
                  << std::setw(14) << std::fixed << std::setprecision(1)
                  << world_.developmentOf(country.tag)
                  << std::setw(14) << std::setprecision(2) << country.treasury
                  << "\n";
    }
    std::cout << "\n";
}

void Game::printProvinces() const {
    std::cout << "\nID   PROVÍNCIA                 DONO    REGIÃO          DESENV.  VIZINHAS\n";
    for (const auto& province : world_.provinces()) {
        const Region* region = world_.findRegion(province.regionId);
        std::cout << std::left << std::setw(5) << province.id
                  << std::setw(26) << province.name
                  << std::setw(8) << (province.ownerTag.empty() ? "---" : province.ownerTag)
                  << std::setw(16) << (region ? region->name : "Inválida")
                  << std::setw(9) << std::fixed << std::setprecision(1)
                  << province.development << province.adjacentProvinceIds.size() << "\n";
    }
    std::cout << "\n";
}

bool Game::save(const std::string& path, std::string& error) const {
    std::ofstream file(path);
    if (!file) {
        error = "Não foi possível criar o arquivo: " + path;
        return false;
    }

    file << "EUROPA_LITE_SAVE 1\n";
    file << date_.year << ' ' << date_.month << ' ' << date_.day << "\n";
    file << world_.countries().size() << "\n";
    for (const auto& country : world_.countries()) {
        file << country.tag << ' ' << country.treasury << ' ' << country.manpower << "\n";
    }
    file << world_.provinces().size() << "\n";
    for (const auto& province : world_.provinces()) {
        file << province.id << ' ' << province.ownerTag << "\n";
    }
    return true;
}

} // namespace eulite
