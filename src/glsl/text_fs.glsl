#version 450 core

uniform sampler2D image;

in vec4 fsColor;
in vec2 fsUv;

out vec4 color;

void main()
{    
    // vec4 sampled = vec4(1.0, 1.0, 1.0, texture(image, fsUv).r);
    // color = vec4(fsColor.rgb, 1.0) * sampled;
    color = vec4(1.0);

    // anti aliasing
//    float d = texture(image, out_uv).r;
//    float aaf = fwidth(d);
//    float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);
//    gl_FragColor = vec4(out_color.rgb, alpha);
}
