#pragma once

#include "world/World.hpp"

#include <string>

namespace eulite {

// Writes a lightweight, dependency-free isometric SVG preview of the current world.
// The layout file is presentation data; it does not alter simulation geography.
bool writeMapPreviewSvg(const World& world,
                        const std::string& layoutPath,
                        const std::string& outputPath,
                        std::string& error);

} // namespace eulite
