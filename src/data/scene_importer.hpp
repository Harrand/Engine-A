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
    /**
     * Get the set of all nodes comprising the given scene.
     * @param scene_element - Root XML element of the scene
     * @return - Set of all node names in the scene
     */
    std::unordered_set<std::string> read_nodes(const tinyxml2::XMLElement* scene_element);
}

/// Scene object taken directly out of some external scene file. Requires processing to obtain the actual StaticObject.
/// Essentially a partially-processed external StaticObject.
struct TextBasedObject
{
    /**
     * Construct a TextBasedObject from a given xml node.
     * @param object_element - The object XML node containing the object data
     */
    TextBasedObject(const tinyxml2::XMLElement* object_element);
    /// The name of the mesh that this object uses.
    std::string mesh_name;
    /// The absolute/relative path to the mesh data on the filesystem (such as "C:/Users/Foo/Meshes/dog.obj")
    std::string mesh_link;
    /// The name of the texture that this object uses.
    std::string texture_name;
    /// The absolute/relative path to the texture image data on the filesystem (such as "C:/Users/Foo/Meshes/dog.png")
    std::string texture_link;
    /// Name of the node containing this object.
    std::string node_name;
    /// Position, rotation and scale of this object, in world-space.
    Transform transform;
};

/// Scene node taken directly out of some external scene file.
/// Required in the PVS deduction segment of the PVSOC algorithm.
struct TextBasedNode
{
    /**
     * Construct a TextBasedNode from a given xml node.
     * @param name - Name of the node
     * @param node_element - The XML node containing this scene node data.
     */
    TextBasedNode(const std::string& name, const tinyxml2::XMLElement* node_element);
    /// Name of the node.
    std::string name;
    /// Set of all nodes which are potentially-visible from any point within this node.
    std::unordered_set<std::string> potentially_visible_set;
};

/// Parses an XML file to create a Scene based upon all objects and nodes in the file.
class SceneImporter
{
public:
    /**
     * Construct a SceneImporter based upon an existing XML file.
     * @param import_filename - The absolute path to the existing XML file on the filesystem
     */
    SceneImporter(std::string import_filename);
    /**
     * Retrieve the Scene as a result from the PVSOC preprocessing.
     * @return - The resultant Scene
     */
    Scene retrieve();
private:
    /**
     * Perform scene preprocessing on the XML file to build the scene objects and nodes.
     */
    void import();
    /// XML Document representing the external scene XML file.
    tinyxml2::XMLDocument import_file;
    /// Buffer containing all assets required for this scene.
    AssetBuffer assets;
    /// Container of all preprocessed scene objects.
    std::vector<TextBasedObject> imported_objects;
    /// Container of all preprocessed scene nodes.
    std::vector<TextBasedNode> imported_nodes;
};

#endif //TOPAZ_SCENE_IMPORTER_HPP
