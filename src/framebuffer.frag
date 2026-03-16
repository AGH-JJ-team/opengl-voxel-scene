#version 330 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

uniform bool invertColors;
uniform bool blackAndWhite;
uniform bool redFilter;
uniform bool greenFilter;
uniform bool blueFilter;
uniform bool border;

const float offset_x = 1.0f / 1920.0f;  
const float offset_y = 1.0f / 1080.0f;

vec2 offsets[9] = vec2[]
(
    vec2(-offset_x,  offset_y), vec2( 0.0f,    offset_y), vec2( offset_x,  offset_y),
    vec2(-offset_x,  0.0f),     vec2( 0.0f,    0.0f),     vec2( offset_x,  0.0f),
    vec2(-offset_x, -offset_y), vec2( 0.0f,   -offset_y), vec2( offset_x, -offset_y) 
);


float kernel[9] = float[]
(
    -1, -1, -1,
    -1, 8, -1,
    -1, -1, -1
);



void main()
{
    vec4 tex = texture(screenTexture, texCoords);
    float avg = (tex.x + tex.y + tex.z) / 3.0f;
    vec3 color = vec3(0.0f);
    
    if (invertColors) {
        vec3 inverted = vec3(1.0) - tex.rgb;
        FragColor = vec4(inverted, tex.a);
    }
    else if (blackAndWhite) {
        FragColor = vec4(avg, avg, avg, 1.0f);
    }
    else if (redFilter) {
        FragColor = vec4(tex.r * 1.5, tex.g * 0.5, tex.b * 0.5, 1.0);
    }
    else if (greenFilter) {
        FragColor = vec4(tex.r * 0.5, tex.g * 1.5, tex.b * 0.5, 1.0);
    }
    else if (blueFilter) {
        FragColor = vec4(tex.r * 0.5, tex.g * 0.5, tex.b * 1.5, 1.0);
    }
    else if (border) {
        for(int i = 0; i < 9; i++) {
            color += vec3(texture(screenTexture, texCoords.st + offsets[i])) * kernel[i];
        }
        FragColor = vec4(color, 1.0f);
    }
    
    else {        
        FragColor = tex;
    }

}