//
// Created by Harry on 08/11/2018.
//
#ifndef TOPAZ_SCENE_IMPORTER_HPP
#define TOPAZ_SCENE_IMPORTER_HPP

#include "data/external/tinyxml2.h"
#include "core/scene.hpp"
#include <string>

namespace tz::ea::importer
{
    constexpr char scene_element_name[] = "scene";
    constexpr char object_element_name[] = "object";
    constexpr char mesh_element_name[] = "mesh";
    constexpr char element_label_name[] = "name";
    constexpr char element_label_path[] = "path";
    constexpr char texture_element_name[] = "texture";
    constexpr char position_element_name[] = "position";
    constexpr char rotation_element_name[] = "rotation";
    constexpr char scale_element_name[] = "scale";
    constexpr char node_element_name[] = "node";
    constexpr char node_set_element_name[] = "node_set";
    constexpr char node_identifier_name[] = "node_";
    constexpr char potentially_visible_set_element_name[] = "pvs";
    std::unordered_set<std::string> read_nodes(const tinyxml2::XMLElement* scene_element);
}

struct TextBasedObject
{
    TextBasedObject(const tinyxml2::XMLElement* object_element);
    std::string mesh_name;
    std::string mesh_link;
    std::string texture_name;
    std::string texture_link;
    std::string node_name;
    Transform transform;
};

struct TextBasedNode
{
    TextBasedNode(const std::string& name, const tinyxml2::XMLElement* node_element);
    std::string name;
    std::unordered_set<std::string> potentially_visible_set;
};

class SceneImporter
{
public:
    SceneImporter(std::string import_filename);
    Scene retrieve();
private:
    void import();
    tinyxml2::XMLDocument import_file;
    AssetBuffer assets;
    std::vector<TextBasedObject> imported_objects;
    std::vector<TextBasedNode> imported_nodes;
};


#endif //TOPAZ_SCENE_IMPORTER_HPP
