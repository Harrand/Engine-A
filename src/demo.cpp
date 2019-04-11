#include "core/listener.hpp"
#include "physics/physics.hpp"
#include "graphics/asset.hpp"
#include "graphics/gui/button.hpp"
#include "graphics/gui/textfield.hpp"
#include "core/scene.hpp"
#include "graphics/skybox.hpp"
#include "core/topaz.hpp"
#include "utility/time.hpp"
#include "graphics/frame_buffer.hpp"
#include "graphics/animated_texture.hpp"
#include "data/scene_importer.hpp"

void init();

int main()
{
    tz::initialise();
    init();
    tz::terminate();
    return 0;
}

void init()
{
    Window wnd("Engine A - PVSOC Demo", 0, 30, 1920, 1080);
    wnd.set_swap_interval_type(Window::SwapIntervalType::IMMEDIATE_UPDATES);

    // During init, enable debug output
    Font font("../../../res/runtime/fonts/Comfortaa-Regular.ttf", 36);
    Label& label = wnd.emplace_child<Label>(Vector2I{100, 50}, font, Vector3F{0.0f, 0.3f, 0.0f}, " ");
    ProgressBar& progress = wnd.emplace_child<ProgressBar>(Vector2I{0, 50}, Vector2I{100, 50}, ProgressBarTheme{{{0.5f, {0.0f, 0.0f, 1.0f}}, {1.0f, {1.0f, 0.0f, 1.0f}}}, {0.1f, 0.1f, 0.1f}}, 0.5f);

    KeyListener key_listener(wnd);
    MouseListener mouse_listener(wnd);
    auto get_node_midpoint = [](const AABB& node_boundary)->Vector3F{return (node_boundary.get_minimum() + node_boundary.get_maximum()) / 2.0f;};

    auto make_button = [&wnd, &font](Vector2I position, std::string title)->Button&{return wnd.emplace_child<Button>(position, Vector2I{100, 50}, font, Vector3F{}, title, Vector3F{0.1f, 0.1f, 0.1f}, Vector3F{0.8f, 0.8f, 0.8f});};
    Button& wireframe_button = wnd.emplace_child<Button>(Vector2I{0, 100}, Vector2I{100, 50}, font, Vector3F{}, "toggle wireframe", Vector3F{0.5f, 0.5f, 0.5f}, Vector3F{0.95f, 0.95f, 0.95f});
    Label& node_label = wnd.emplace_child<Label>(Vector2I{330, 100}, font, Vector3F{0.3f, 0.1f, 0.0f}, " ");
    Button& maze1_button = make_button({0, 150}, "Maze 1");
    Button& maze2_button = make_button({0, 200}, "Maze 2");
    Button& maze3_button = make_button({0, 250}, "Maze 3");
    Button& dynamic1_button = make_button({0, 300}, "Dynamic 1");
    Button& dynamic2_button = make_button({0, 350}, "Dynamic 2");
    Button& dynamic3_button = make_button({0, 400}, "Dynamic 3");
    Button& terrain1_button = make_button({0, 450}, "Terrain 1");

    std::vector<Vector3F> terrain1_midpoints;
    for(float x = 0; x < 220; x += 20)
    {
        for(float y = 0; y < 220; y += 20)
        {
            terrain1_midpoints.emplace_back(x, 1.0f, y);
        }
    }

    bool wireframe = false;
    wireframe_button.set_callback([&wireframe](){wireframe = !wireframe;});

    constexpr float speed = 0.5f;
    Shader render_shader("../../../src/shaders/3D_FullAssets");

    Shader gui_shader("../../../src/shaders/Gui");
    Shader hdr_gui_shader("../../../src/shaders/Gui_HDR");
    Camera camera;
    camera.position = {0, 0, 0};

    SceneImporter importer1{"maze1.xml"};
    SceneImporter importer2{"maze2.xml"};
    SceneImporter importer3{"maze3.xml"};
    SceneImporter importer4{"terrain1.xml"};
    Scene scene1 = importer1.retrieve();
    Scene scene2 = importer2.retrieve();
    Scene scene3 = importer3.retrieve();
    Scene dynamic1 = importer1.retrieve();
    Scene dynamic2 = importer2.retrieve();
    Scene dynamic3 = importer3.retrieve();
    Scene terrain1 = importer4.retrieve();
    Scene* scene = &scene1;

    auto get_height = [&]()->float
    {
        if(scene == &scene1 || scene == &dynamic1)
            return 100.0f;
        else if(scene == &scene2 || scene == &dynamic2)
            return 1000.0f;
        else if(scene == &scene3 || scene == &dynamic3)
            return 10.0f;
        else
            return 0.0f;
    };

    bool dynamic = false;

    maze1_button.set_callback([&scene, &scene1, &camera, &dynamic](){scene = &scene1; camera.position = {110, 100, -110}; dynamic = false;});
    maze2_button.set_callback([&scene, &scene2, &camera, &dynamic](){scene = &scene2; camera.position = {1100, 1000, -1100}; dynamic = false;});
    maze3_button.set_callback([&scene, &scene3, &camera, &dynamic](){scene = &scene3; camera.position = {11, 10, -11}; dynamic = false;});

    dynamic1_button.set_callback([&scene, &dynamic1, &camera, &dynamic](){scene = &dynamic1; camera.position = {110, 100, -110}; dynamic = true;});
    dynamic2_button.set_callback([&scene, &dynamic2, &camera, &dynamic](){scene = &dynamic2; camera.position = {1100, 1000, -1100}; dynamic = true;});
    dynamic3_button.set_callback([&scene, &dynamic3, &camera, &dynamic](){scene = &dynamic3; camera.position = {11, 10, -11}; dynamic = true;});

    terrain1_button.set_callback([&scene, &terrain1, &camera, &dynamic](){scene = &terrain1; camera.position = {100, 100, 110}; dynamic = false;});

    AssetBuffer assets;
    assets.emplace<Mesh>("cube_lq", "../../../res/runtime/models/cube.obj");
    assets.emplace<Mesh>("cube", "../../../res/runtime/models/cube_hd.obj");
    assets.emplace<Mesh>("monkey", "../../../res/runtime/models/monkeyhead.obj");
    assets.emplace<Mesh>("cylinder", "../../../res/runtime/models/cylinder.obj");
    assets.emplace<Mesh>("sphere", "../../../res/runtime/models/sphere.obj");
    assets.emplace<Mesh>("plane_hd", "../../../res/runtime/models/plane_hd.obj");
    assets.emplace<Texture>("bricks", "../../../res/runtime/textures/bricks.jpg");
    assets.emplace<Texture>("stone", "../../../res/runtime/textures/stone.jpg");
    assets.emplace<Texture>("wood", "../../../res/runtime/textures/wood.jpg");
    assets.emplace<NormalMap>("bricks_normal", "../../../res/runtime/normalmaps/bricks_normalmap.jpg");
    assets.emplace<NormalMap>("stone_normal", "../../../res/runtime/normalmaps/stone_normalmap.jpg");
    assets.emplace<NormalMap>("wood_normal", "../../../res/runtime/normalmaps/wood_normalmap.jpg");
    assets.emplace<ParallaxMap>("bricks_parallax", "../../../res/runtime/parallaxmaps/bricks_parallax.jpg");
    assets.emplace<ParallaxMap>("stone_parallax", "../../../res/runtime/parallaxmaps/stone_parallax.png", 0.06f, -0.5f);
    assets.emplace<ParallaxMap>("wood_parallax", "../../../res/runtime/parallaxmaps/wood_parallax.jpg");
    assets.emplace<DisplacementMap>("bricks_displacement", "../../../res/runtime/displacementmaps/bricks_displacement.png");
    assets.emplace<DisplacementMap>("noise_displacement", tz::graphics::height_map::generate_cosine_noise(256, 256, 100.0f));

    long long int time = tz::utility::time::now();
    Timer second_timer, tick_timer;
    std::vector<float> pvsoc_deltas;
    TimeProfiler profiler;
    using namespace tz::graphics;
    while(!wnd.is_close_requested())
    {
        profiler.begin_frame();
        static float x = 0;
        progress.set_progress((1 + std::sin(x += 0.01)) / 2.0f);
        second_timer.update();
        tick_timer.update();
        static bool should_report = false;
        if(second_timer.millis_passed(1000.0f))
        {
            using namespace tz::utility::generic::cast;
            label.set_text(to_string(profiler.get_delta_average()) + " ms (" + to_string(profiler.get_fps()) + " fps)");
            second_timer.reload();
            // print out time deltas for profiling.
            static std::size_t cumulative_frame_id;
            if(should_report)
            {
                for (std::size_t frame_id = 0; frame_id < profiler.deltas.size(); frame_id++)
                {
                    //std::cout << (cumulative_frame_id + frame_id) << ", " << profiler.deltas[frame_id] << ", " << scene->get_node_containing_position(camera.position).value_or("No Node") << "\n"; // total frame time
                    std::cout << (cumulative_frame_id + frame_id) << ", " << pvsoc_deltas[frame_id] << ", " << scene->get_node_containing_position(camera.position).value_or("No Node") << "\n"; // pvsoc only
                }
                cumulative_frame_id += profiler.deltas.size();
                pvsoc_deltas.clear();
                should_report = false;
            }
            profiler.reset();
            node_label.set_text(scene->get_node_containing_position(camera.position).value_or("No Node"));
        }

        long long int delta_time = tz::utility::time::now() - time;
        time = tz::utility::time::now();
        wnd.set_render_target();
        wnd.clear();
        if(wireframe)
            tz::graphics::enable_wireframe_render(true);
        scene->render(&render_shader, &gui_shader, camera, {wnd.get_width(), wnd.get_height()});
        pvsoc_deltas.push_back(scene->get_pvsoc_time_this_frame(true));
        constexpr int tps = 120;
        constexpr float tick_delta = 1000.0f / tps;
        if(tick_timer.millis_passed(tick_delta))
        {
            scene->update(tick_delta / 1000.0f);
            if(dynamic)
            {
                // if we're in scene2, continue moving all the objects around.
                static int x = 0;
                for(const StaticObject& object : scene->get_static_objects())
                    object.transform.scale.y = get_height() * std::abs(std::sin(std::cbrt(3 + static_cast<int>(object.transform.position.x) % static_cast<int>(scene->get_boundary().get_maximum().x - scene->get_boundary().get_minimum().x)) * ++x * 0.0002f));
            }
            tick_timer.reload();
        }
        if(wireframe)
            tz::graphics::enable_wireframe_render(false);
        wnd.update(gui_shader, &hdr_gui_shader);

        if(mouse_listener.is_left_clicked())
        {
            Vector2F delta = mouse_listener.get_mouse_delta_position();
            camera.rotation.y += 0.03 * delta.x;
            camera.rotation.x += 0.03 * delta.y;
            mouse_listener.reload_mouse_delta();
        }
        if(key_listener.is_key_pressed("Escape"))
            break;
        if(key_listener.is_key_pressed("W"))
            camera.position += camera.forward() * delta_time * speed;
        if(key_listener.is_key_pressed("S"))
            camera.position += camera.backward() * delta_time * speed;
        if(key_listener.is_key_pressed("A"))
            camera.position += camera.left() * delta_time * speed;
        if(key_listener.is_key_pressed("D"))
            camera.position += camera.right() * delta_time * speed;
        static std::size_t terrain_node_counter = 0;
        if(key_listener.catch_key_pressed("P"))
        {
            if(scene == &terrain1)
            {
                if(--terrain_node_counter < 0)
                    terrain_node_counter = terrain1_midpoints.size() - 1;
                camera.position = terrain1_midpoints[terrain_node_counter];
            }
            else
            {
                std::optional < std::string > node = scene->get_node_containing_position(camera.position);
                const std::string final_node = *std::max_element(scene->get_nodes().begin(),
                                                                 scene->get_nodes().end());//*(std::next(scene->get_nodes().begin()));
                if (node.has_value() && node.value() > "A")
                {
                    std::string node_v = {--node.value()[0]};
                    //std::cout << "moving to node " << node_v << " midpoint.\n";
                    camera.position = get_node_midpoint(scene->get_node_bounding_box(node_v).value());
                } else
                {
                    //std::cout << "resetting to node " << final_node << " midpoint.\n";
                    camera.position = get_node_midpoint(scene->get_node_bounding_box(final_node).value());
                }
            }
        }
        if(key_listener.catch_key_pressed("N"))
        {
            if(scene == &terrain1)
            {
                if(++terrain_node_counter >= terrain1_midpoints.size())
                    terrain_node_counter = 0;
                camera.position = terrain1_midpoints[terrain_node_counter];
            }
            else
            {
                std::optional < std::string > node = scene->get_node_containing_position(camera.position);
                const std::string final_node = *std::max_element(scene->get_nodes().begin(),
                                                                 scene->get_nodes().end());//*(std::next(scene->get_nodes().begin()));
                if (node.has_value() && node.value() < final_node)
                {
                    std::string node_v = {++node.value()[0]};
                    //std::cout << "moving to node " << node_v << " midpoint.\n";
                    camera.position = get_node_midpoint(scene->get_node_bounding_box(node_v).value());
                } else {
                    //std::cout << "resetting to node A midpoint.\n";
                    camera.position = get_node_midpoint(scene->get_node_bounding_box("A").value());
                }
            }
        }
        if(key_listener.catch_key_pressed("R"))
        {
            should_report = true;
            /*
            for(const auto& node : scene->get_nodes())
            {
                std::cout << "node " << node << " midpoint = " << get_node_midpoint(scene->get_node_bounding_box(node).value()) << ".\n";
            }
            */
        }
        /*
        if(key_listener.is_key_pressed("Up"))
            example_sprite.position_screenspace.y += 3;
        if(key_listener.is_key_pressed("Down"))
            example_sprite.position_screenspace.y -= 3;
        if(key_listener.is_key_pressed("Left"))
            example_sprite.position_screenspace.x -= 3;
        if(key_listener.is_key_pressed("Right"))
            example_sprite.position_screenspace.x += 3;
            */
        profiler.end_frame();
    }
}