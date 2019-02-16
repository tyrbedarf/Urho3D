option (URHO3D_IMGUI "Enable SystemUi (Dear ImGUI)" TRUE)
option (URHO3D_TURBO_BADGER "Enable Turbo Badger" TRUE)

# Define preprocessor macros (for building the Urho3D library) based on the configured build options
foreach (OPT
        URHO3D_IMGUI
        URHO3D_TURBO_BADGER)
    if (${OPT})
        add_definitions (-D${OPT})
    endif ()
endforeach ()

if (URHO3D_IMGUI)
        add_subdirectory (ThirdParty/imgui)
endif()

if (URHO3D_TURBO_BADGER)
        add_subdirectory (ThirdParty/TurboBadger)
endif()