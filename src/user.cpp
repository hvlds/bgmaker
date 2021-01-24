#include "user.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <toml.hpp>
#include "fractal_plant.hpp"
#include "fractal_binary_tree.hpp"

UserInfo::UserInfo() {
    this->general_info.height = 1080;
    this->general_info.width = 1920;
    this->is_preview = false;
    this->stroke_color = "#FFFFFF";
    this->general_info.background_color = "#546E7A";
    this->general_info.output_file = "background.png";
}

void UserInfo::create_empty_canvas() {
    this->image = std::make_shared<Magick::Image>(
        Magick::Geometry(this->general_info.width, this->general_info.height),
        Magick::Color(this->general_info.background_color));
}

void UserInfo::read_config(std::string config_path) {
    const auto data = toml::parse(config_path);
    if (data.contains("general")) {
        auto general_config = toml::find<config::General>(data, "general");
        this->general_info = general_config;
    }
    
    for (auto drawing_option : this->drawing_options) {
        if (data.contains(drawing_option)) {
            const auto& lsystems = toml::find(data, drawing_option);
            auto key = drawing_option;
            std::vector<config::Drawable> drawable_configs;
            for (const auto& item : lsystems.as_table()) {
                std::cout << item.first << std::endl;
                auto drawable_config = toml::find<config::Drawable>(lsystems, item.first);
                drawable_configs.push_back(drawable_config);
            }
            this->drawable_config.insert(
                {key, drawable_configs});
        }
    }
}

void UserInfo::draw() {
    int width = this->general_info.width;
    int height = this->general_info.height;

    for (auto item : this->drawable_config) {
        auto name = item.first;
        auto configs = item.second;
        int offset = 500;
        if (configs.size() > 0 && name == "fractal-plant") {
            for (auto config : configs) {
                FractalPlant fplant = FractalPlant(config);
                fplant.draw(this->image, width - offset, height);
                offset += 100;
            }
        } else if (configs.size() > 0 && name == "fractal-binary-tree") {
            for (auto config : configs) {
                offset += 500;
                FractalBinaryTree fbtree = FractalBinaryTree(config);
                fbtree.draw(this->image, width + offset, height);
            }
        }
    }
}