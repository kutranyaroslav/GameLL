uniform sampler2D texture;
uniform vec2  resolution;
uniform float time;

// Phase 1
uniform float vignetteStrength;
uniform float grainStrength;

// Phase 2
uniform float aberrationStrength; // 0 = выкл, ~0.002-0.006 заметно
uniform float scanlineStrength;   // 0 = выкл, ~0.1-0.25 заметно
uniform vec3  lift;               // тени,   нейтраль (0,0,0)
uniform vec3  gamma;              // полутона, нейтраль (1,1,1)
uniform vec3  gain;               // света,  нейтраль (1,1,1)

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453 + time);
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;

    // хроматическая аберрация: R и B расходятся от центра, G остаётся на месте
    vec2 dir = uv - vec2(0.5);
    vec2 offset = dir * aberrationStrength;
    float r = texture2D(texture, uv - offset).r;
    float g = texture2D(texture, uv).g;
    float b = texture2D(texture, uv + offset).b;
    vec4 color = vec4(r, g, b, 1.0);

    // цветокоррекция: тени -> полутона -> света
    color.rgb = color.rgb + lift * (1.0 - color.rgb);
    color.rgb = pow(max(color.rgb, 0.0), 1.0 / gamma);
    color.rgb = color.rgb * gain;

    // виньетка (с поправкой на аспект)
    vec2 pos = uv - vec2(0.5);
    pos.x *= resolution.x / resolution.y;
    float vig = 1.0 - dot(pos, pos) * vignetteStrength;
    color.rgb *= clamp(vig, 0.0, 1.0);

    // сканлайны
    float scan = 0.5 + 0.5 * sin(uv.y * resolution.y * 3.14159265);
    color.rgb *= mix(1.0, scan, scanlineStrength);

    // зерно
    float grain = rand(uv * resolution) - 0.5;
    color.rgb += grain * grainStrength;

    gl_FragColor = color * gl_Color;
}