//
// Created by Harry on 08/11/2018.
//

#include "scene_importer.hpp"

TextBasedObject::TextBasedObject(const tinyxml2::XMLElement* object_element): mesh_name(object_element->FirstChildElement(tz::ea::importer::mesh_element_name)->FirstChildElement(tz::ea::importer::element_label_name)->GetText()), mesh_link(object_element->FirstChildElement(tz::ea::importer::mesh_element_name)->FirstChildElement(tz::ea::importer::element_label_path)->GetText()), texture_name(object_element->FirstChildElement(tz::ea::importer::texture_element_name)->FirstChildElement(tz::ea::importer::element_label_name)->GetText()), texture_link(object_element->FirstChildElement(tz::ea::importer::texture_element_name)->FirstChildElement(tz::ea::importer::element_label_path)->GetText()), transform{{}, {}, {}}
{
    using namespace tz::utility::generic;

    const tinyxml2::XMLElement* element = object_element->FirstChildElement(tz::ea::importer::position_element_name);
    this->transform.position.x = cast::from_string<float>(std::string(element->FirstChildElement("x")->GetText()));
    this->transform.position.y = cast::from_string<float>(std::string(element->FirstChildElement("y")->GetText()));
    this->transform.position.z = cast::from_string<float>(std::string(element->FirstChildElement("z")->GetText()));

    element = object_element->FirstChildElement(tz::ea::importer::rotation_element_name);
    this->transform.rotation.x = cast::from_string<float>(std::string(element->FirstChildElement("x")->GetText()));
    this->transform.rotation.y = cast::from_string<float>(std::string(element->FirstChildElement("y")->GetText()));
    this->transform.rotation.z = cast::from_string<float>(std::string(element->FirstChildElement("z")->GetText()));

    element = object_element->FirstChildElement(tz::ea::importer::scale_element_name);
    this->transform.scale.x = cast::from_string<float>(std::string(element->FirstChildElement("x")->GetText()));
    this->transform.scale.y = cast::from_string<float>(std::string(element->FirstChildElement("y")->GetText()));
    this->transform.scale.z = cast::from_string<float>(std::string(element->FirstChildElement("z")->GetText()));
}

SceneImporter::SceneImporter(std::string import_filename): import_file(), assets(), imported_objects()
{
    this->import_file.LoadFile(import_filename.c_str());
    if(!this->import_file.Error())
        this->import();
}

Scene SceneImporter::retrieve()
{
    Scene scene;
    for(const TextBasedObject& object : this->imported_objects)
    {
        std::cout << "begin loop.\n";
        std::cout << "names: " << object.mesh_name << ", " << object.texture_name << "\n";
        std::cout << "links: " << object.mesh_link << ", " << object.texture_link << "\n";
        if(this->assets.find_mesh(object.mesh_name) == nullptr)
        {
            std::cout << "emplacing new mesh...\n";
            this->assets.emplace_mesh(object.mesh_name, object.mesh_link);
            std::cout << "emplaced.\n";
        }
        if(this->assets.find_texture(object.texture_name) == nullptr)
            this->assets.emplace_texture(object.texture_name, object.texture_link);
        std::cout << "time to emplace the object.\n";
        scene.emplace_object(object.transform, Asset{this->assets.find_mesh(object.mesh_name), this->assets.find_texture(object.texture_name)});
        std::cout << "end loop.\n";
    }
    return scene;
}

void SceneImporter::import()
{
    using element = const tinyxml2::XMLElement;
    this->imported_objects.clear();
    element* scene_element = this->import_file.FirstChildElement(tz::ea::importer::scene_element_name);
    std::size_t child_counter = 0;
    auto get_current_name = [&child_counter]()->std::string{return tz::ea::importer::object_element_name + std::to_string(child_counter);};
    element* object_element = nullptr;
    do
    {
        object_element = scene_element->FirstChildElement(get_current_name().c_str());
        std::cout << "object element = " << object_element << "\n";
        if(object_element != nullptr)
            this->imported_objects.emplace_back(object_element);
        std::cout << "coon.\n";
        child_counter++;
    }while(object_element != nullptr);
}