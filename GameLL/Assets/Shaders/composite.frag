uniform sampler2D texture;
uniform vec2  resolution;
uniform float time;
uniform float vignetteStrength; // 0 = выкл, ~0.3-0.6 — заметно
uniform float grainStrength;    // 0 = выкл, ~0.03-0.08 — заметно

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453 + time);
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec4 color = texture2D(texture, uv);

    // виньетка: параболическое затухание от центра, с поправкой на аспект
    vec2 pos = uv - vec2(0.5);
    pos.x *= resolution.x / resolution.y;
    float vig = 1.0 - dot(pos, pos) * vignetteStrength;
    color.rgb *= clamp(vig, 0.0, 1.0);

    // зерно: псевдошум на пиксель, дрожит от time
    float grain = rand(uv * resolution) - 0.5;
    color.rgb += grain * grainStrength;

    gl_FragColor = color * gl_Color;
}