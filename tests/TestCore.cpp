#include "core/Date.hpp"
#include "world/World.hpp"

#include <cmath>
#include <iostream>
#include <string>

namespace {

bool check(bool condition, const std::string& message) {
    if (!condition) std::cerr << "FALHOU: " << message << "\n";
    return condition;
}

} // namespace

int main() {
    bool passed = true;

    eulite::GameDate date;
    passed &= check(date.toString() == "11/11/1444", "data inicial");
    date.advanceMonth();
    passed &= check(date.toString() == "11/12/1444", "avanço de novembro para dezembro");
    date.advanceMonth();
    passed &= check(date.toString() == "11/01/1445", "virada de ano");

    eulite::World world;
    std::string error;
    const std::string dataPath = std::string(EULITE_SOURCE_DIR) + "/data";
    passed &= check(world.load(dataPath, error), "carregamento do cenário: " + error);
    passed &= check(world.countries().size() == 4, "quantidade inicial de países");
    passed &= check(world.provinces().size() == 10, "quantidade inicial de províncias");
    passed &= check(world.findCountry("FRA") != nullptr, "busca de país por tag");
    passed &= check(world.countOwnedProvinces("CAS") == 3, "contagem de províncias de Castela");
    passed &= check(std::abs(world.developmentOf("FRA") - 21.0) < 0.0001,
                    "desenvolvimento total da França");

    if (!passed) return 1;
    std::cout << "Todos os testes do núcleo passaram.\n";
    return 0;
}
