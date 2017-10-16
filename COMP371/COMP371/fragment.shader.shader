#version 330 core

out vec4 color;
uniform int object_type;

void main()
{
	if (object_type == 0) {
		color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	if (object_type == 1) {
		color = vec4(0.0f, 0.5f, 0.0f, 1.0f);
	}
	if (object_type == 2) {
		color = vec4(0.75f, 0.25f, 0.25f, 1.0f);
	}
	if (object_type == 3) {
		color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	}
	if (object_type == 4) {
		color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
if (object_type == 5) {
		color = vec4(0.5f, 0.5f, 0.0f, 1.0f);
	}
if (object_type == 6) {
		color = vec4(0.75f, 1.0f, 0.5f, 1.0f);
	}
if (object_type == 7) {
		color = vec4(0.75f, 0.25f, 1.0f, 1.0f);
	}
}