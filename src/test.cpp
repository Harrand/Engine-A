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
    Window wnd("Engine A Development Window", 0, 30, 1920, 1080);
    wnd.set_fullscreen(Window::FullscreenType::DESKTOP_MODE);
    wnd.set_swap_interval_type(Window::SwapIntervalType::IMMEDIATE_UPDATES);

    // During init, enable debug output
    Font font("../../../res/runtime/fonts/Comfortaa-Regular.ttf", 36);
    Label& label = wnd.emplace_child<Label>(Vector2I{100, 50}, font, Vector3F{0.0f, 0.3f, 0.0f}, " ");
    ProgressBar& progress = wnd.emplace_child<ProgressBar>(Vector2I{0, 50}, Vector2I{100, 50}, ProgressBarTheme{{{0.5f, {0.0f, 0.0f, 1.0f}}, {1.0f, {1.0f, 0.0f, 1.0f}}}, {0.1f, 0.1f, 0.1f}}, 0.5f);

    KeyListener key_listener(wnd);
    MouseListener mouse_listener(wnd);

    Button& wireframe_button = wnd.emplace_child<Button>(Vector2I{0, 100}, Vector2I{100, 50}, font, Vector3F{}, "toggle wireframe", Vector3F{0.1f, 0.1f, 0.1f}, Vector3F{0.8f, 0.8f, 0.8f});
    bool wireframe = false;
    wireframe_button.set_callback([&wireframe](){wireframe = !wireframe;});

    constexpr float speed = 0.5f;
    Shader render_shader("../../../src/shaders/3D_FullAssets");

    Shader gui_shader("../../../src/shaders/Gui");
    Shader hdr_gui_shader("../../../src/shaders/Gui_HDR");
    Camera camera;
    camera.position = {0, 0, 0};

    SceneImporter test0{"test_scene.xml"};
    Scene scene = test0.retrieve();

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
    TimeProfiler profiler;
    using namespace tz::graphics;
    while(!wnd.is_close_requested())
    {
        profiler.begin_frame();
        static float x = 0;
        progress.set_progress((1 + std::sin(x += 0.01)) / 2.0f);
        second_timer.update();
        tick_timer.update();
        if(second_timer.millis_passed(1000.0f))
        {
            using namespace tz::utility::generic::cast;
            label.set_text(to_string(profiler.get_delta_average()) + " ms (" + to_string(profiler.get_fps()) + " fps)");
            second_timer.reload();
            profiler.reset();
        }

        long long int delta_time = tz::utility::time::now() - time;
        time = tz::utility::time::now();
        wnd.set_render_target();
        wnd.clear();
        if(wireframe)
            tz::graphics::enable_wireframe_render(true);
        scene.render(&render_shader, &gui_shader, camera, {wnd.get_width(), wnd.get_height()});
        constexpr int tps = 120;
        constexpr float tick_delta = 1000.0f / tps;
        if(tick_timer.millis_passed(tick_delta))
        {
            scene.update(tick_delta / 1000.0f);
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