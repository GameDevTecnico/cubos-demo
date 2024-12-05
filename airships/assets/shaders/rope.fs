in vec3 fragPosition;
in vec3 fragNormal;
in vec3 fragAlbedo;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 albedo;

void main()
{
    position = fragPosition;
    normal = fragNormal;
    albedo = fragAlbedo;
}
