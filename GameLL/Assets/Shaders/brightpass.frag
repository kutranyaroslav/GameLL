uniform sampler2D texture;
uniform float threshold; // ниже этой яркости — в bloom не идёт

void main() {
    vec4 color = texture2D(texture, gl_TexCoord[0].xy);
    float luminance = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    float contribution = max(luminance - threshold, 0.0) / max(luminance, 0.0001);
    gl_FragColor = vec4(color.rgb * contribution, 1.0) * gl_Color;
}