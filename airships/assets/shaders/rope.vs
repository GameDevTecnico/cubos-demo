in vec3 position;
in vec3 normal;

out vec3 fragPosition;
out vec3 fragNormal;
out vec3 fragAlbedo;

uniform PerScene
{
    mat4 viewProj;
};

uniform PerRope
{
    mat4 model;
    vec4 color;
};

void main(void)
{
    vec4 worldPosition = model * vec4(position, 1.0);
    fragPosition = vec3(worldPosition);
    gl_Position = viewProj * worldPosition;

    mat3 N = transpose(inverse(mat3(model)));
    fragNormal = normalize(N * normal);

    fragAlbedo = color.rgb;
}
