uniform sampler2D texture;
uniform vec2 direction; // (1/width, 0) для H, (0, 1/height) для V

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec4 sum = vec4(0.0);
    sum += texture2D(texture, uv - direction * 4.0) * 0.0162162162;
    sum += texture2D(texture, uv - direction * 3.0) * 0.0540540541;
    sum += texture2D(texture, uv - direction * 2.0) * 0.1216216216;
    sum += texture2D(texture, uv - direction * 1.0) * 0.1945945946;
    sum += texture2D(texture, uv)                   * 0.2270270270;
    sum += texture2D(texture, uv + direction * 1.0) * 0.1945945946;
    sum += texture2D(texture, uv + direction * 2.0) * 0.1216216216;
    sum += texture2D(texture, uv + direction * 3.0) * 0.0540540541;
    sum += texture2D(texture, uv + direction * 4.0) * 0.0162162162;
    gl_FragColor = sum * gl_Color;
}