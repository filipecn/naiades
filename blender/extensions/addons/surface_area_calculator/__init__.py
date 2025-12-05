# Blender Add-on: Surface Area Calculator & Visualizer
#
# This script creates a simple panel in the 3D Viewport sidebar
# and an operator to calculate the total surface area and paint
# mesh faces based on their individual area (Blue=Small, Red=Large).
#
# To install:
# 1. Save this file as 'surface_area_calculator.py'.
# 2. In Blender, go to Edit -> Preferences -> Add-ons.
# 3. Click 'Install...' and select the saved file.
# 4. Enable the add-on by checking the box.
# 5. The panel will appear in the 3D Viewport's sidebar (N-key) under the 'Surface Area' tab.

import bpy
import bmesh
from bpy.props import FloatProperty
from mathutils import Color  # Added for color manipulation

bl_info = {
    "name": "Surface Area Visualizer",
    "author": "Gemini",
    "version": (1, 1),
    "blender": (4, 0, 0),
    "location": "3D Viewport > Sidebar > Surface Area",
    "description": "Calculates and visually paints faces based on surface area.",
    "category": "Mesh",
}

# --------------------------------------------------------------------------
# 1. Operator: Calculate Surface Area & Paint
# --------------------------------------------------------------------------


class OBJECT_OT_calculate_surface_area(bpy.types.Operator):
    """Calculate the total surface area and paint faces based on their area."""

    bl_idname = "object.calculate_surface_area"
    bl_label = "Calculate & Paint Area"
    bl_options = {"REGISTER", "UNDO"}

    result_area: FloatProperty(
        name="Calculated Area",
        description="The resulting total surface area.",
        default=0.0,
    )

    def execute(self, context):
        obj = context.active_object

        if obj is None or obj.type != "MESH":
            self.report({"ERROR"}, "No active mesh object selected.")
            return {"CANCELLED"}

        # Ensure object is in Object Mode for safe mesh data access and manipulation
        original_mode = context.mode
        if original_mode != "OBJECT":
            try:
                bpy.ops.object.mode_set(mode="OBJECT")
            except RuntimeError:
                self.report({"ERROR"}, "Could not switch to Object Mode.")
                return {"CANCELLED"}

        mesh = obj.data
        bm = bmesh.new()
        bm.from_mesh(mesh)

        # 1. Calculate all face areas and find min/max
        face_areas = {}
        total_area = 0.0
        min_area = float("inf")
        max_area = float("-inf")

        for face in bm.faces:
            area = face.calc_area()
            face_areas[face] = area
            total_area += area

            min_area = min(min_area, area)
            max_area = max(max_area, area)

        self.result_area = total_area

        area_range = max_area - min_area
        # Prevent division by zero if all faces have the exact same area
        if area_range < 1e-6:
            area_range = 1.0
            min_area = 0.0

        # 2. Create or reuse the color attribute (Vertex Color Layer)
        color_attribute_name = "Area_Color"

        # Remove old attribute if it exists for a clean slate
        if color_attribute_name in mesh.color_attributes:
            mesh.color_attributes.remove(mesh.color_attributes[color_attribute_name])

        # Create a new Color Attribute (FLOAT_COLOR for precision, DOMAIN='CORNER' for face-based color)
        color_attribute = mesh.color_attributes.new(
            name=color_attribute_name, type="FLOAT_COLOR", domain="CORNER"
        )

        # Get the attribute access layer for the BMesh
        color_layer = bm.loops.layers.color.new(color_attribute_name)

        # 3. Apply color to faces based on normalized area (Blue to Red gradient)
        for face in bm.faces:
            area = face_areas[face]

            # Normalize area to a 0.0 to 1.0 range
            if area_range == 1.0:
                normalized_area = 0.5
            else:
                normalized_area = (area - min_area) / area_range

            # Simple Blue (0) to Red (1) gradient:
            # R increases with size, B decreases with size. G is always 0.
            r = normalized_area  # Red component (larger faces)
            g = 0.0  # Green component
            b = 1.0 - normalized_area  # Blue component (smaller faces)

            # Apply the color to all face corners (loops)
            for loop in face.loops:
                loop[color_layer] = (r, g, b, 1.0)  # RGBA

        # 4. Write the modified BMesh data (including color attribute changes) back to the mesh
        bm.to_mesh(mesh)
        mesh.update()
        bm.free()

        # Set the viewport display mode to see the color attribute immediately
        try:
            # Set the new color attribute as the active one for display
            obj.data.attributes.active_color = obj.data.attributes[color_attribute_name]

            # Switch to Vertex Paint mode for immediate visual confirmation
            bpy.ops.object.mode_set(mode="VERTEX_PAINT")

            result_msg = f"Area: {total_area:.4f} m². Faces painted (Blue=Small, Red=Large). Switched to Vertex Paint."

        except KeyError:
            result_msg = f"Area: {total_area:.4f} m². Faces painted, but failed to set active attribute."
        except RuntimeError:
            result_msg = (
                f"Area: {total_area:.4f} m². Faces painted, but mode switch failed."
            )

        # 5. Report the result
        print(result_msg)
        self.report({"INFO"}, result_msg)

        return {"FINISHED"}


# --------------------------------------------------------------------------
# 2. Panel: Sidebar UI
# --------------------------------------------------------------------------


class VIEW3D_PT_surface_area_panel(bpy.types.Panel):
    """Creates a Panel in the 3D Viewport Sidebar (N-key menu)"""

    bl_label = "Surface Area Visualizer"
    bl_idname = "VIEW3D_PT_surface_area_panel"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "Surface Area"  # The name of the tab in the sidebar

    def draw(self, context):
        layout = self.layout

        # Check if the active object is a mesh
        obj = context.active_object
        is_mesh = obj and obj.type == "MESH"

        if not is_mesh:
            layout.label(text="Select an active Mesh Object", icon="INFO")
            return

        # Display the name of the object being analyzed
        layout.label(text=f"Object: {obj.name}", icon="MESH_DATA")

        # Add the button to run the operator
        row = layout.row()
        row.scale_y = 1.2
        # Using a reliable icon
        row.operator(
            OBJECT_OT_calculate_surface_area.bl_idname,
            text="Calculate & Paint Area",
        )

        # We'll rely on the operator reporting the value to the console/info bar for simplicity.
        layout.separator()
        layout.label(text="Result (See Console/Info Bar)", icon="FILE_CACHE")

        if obj and obj.type == "MESH":
            layout.label(text="Visualization requires:")
            layout.label(text="- Material with Color Attribute node.", icon="MATERIAL")


# --------------------------------------------------------------------------
# 3. Registration
# --------------------------------------------------------------------------

classes = (
    OBJECT_OT_calculate_surface_area,
    VIEW3D_PT_surface_area_panel,
)


def register():
    """Called by Blender when the add-on is enabled."""
    for cls in classes:
        bpy.utils.register_class(cls)
    print(f"{bl_info['name']} registered.")


def unregister():
    """Called by Blender when the add-on is disabled."""
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)
    print(f"{bl_info['name']} unregistered.")


if __name__ == "__main__":
    # If run directly from Blender's text editor, it will register/unregister cleanly.
    # Check if we are already registered to prevent errors.
    if "OBJECT_OT_calculate_surface_area" in dir(bpy.types):
        unregister()
    register()
    if "OBJECT_OT_calculate_surface_area" in dir(bpy.types):
        unregister()
    register()
