#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform float u_time;
uniform vec2 u_resolution;

// A pseudo-random hash function
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// Basic 2D Value Noise
float noise(in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// Fractional Brownian Motion (layers of noise)
float fbm(in vec2 st) {
    float value = 0.0;
    float amplitude = 0.5;
    vec2 shift = vec2(100.0);
    // Rotate to reduce predictable patterns
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
    for (int i = 0; i < 5; ++i) {
        value += amplitude * noise(st);
        st = rot * st * 2.0 + shift;
        amplitude *= 0.5;
    }
    return value;
}

void main() {
    // Normalize coordinates and fix aspect ratio
    vec2 st = gl_FragCoord.xy / u_resolution.xy;
    st.x *= u_resolution.x / u_resolution.y; 

    // Animate the coordinates to make the fog slowly drift and warp
    // Multiply u_time by small numbers to make it agonizingly slow
    vec2 q = vec2(0.);
    q.x = fbm(st + 0.00 * u_time);
    q.y = fbm(st + vec2(1.0));

    vec2 r = vec2(0.);
    r.x = fbm(st + 1.0 * q + vec2(1.7, 9.2) + 0.10 * u_time);
    r.y = fbm(st + 1.0 * q + vec2(8.3, 2.8) + 0.08 * u_time);

    float f = fbm(st + r);

    // --- THE GRIMDARK PALETTE ---
    // Mix between absolute crushing black and a very dull, dying ash color
    vec3 colorBase = vec3(0.01, 0.01, 0.01); // The Void
    vec3 colorAsh = vec3(0.08, 0.07, 0.1);  // Dying dust
    vec3 colorGray = vec3(0.120, 0.116, 0.116);
    vec3 color = mix(colorBase, colorGray, clamp((f * f) * 3.0, 0.0, 1.0));

    // Optional: Add a heavy vignette to fade the edges into total pitch black
    float vignette = 1.0 - smoothstep(0.4, 1.2, length(st - vec2(0.5 * (u_resolution.x/u_resolution.y), 0.5)));
    color *= vignette;

    finalColor = vec4(color, 1.0);
}