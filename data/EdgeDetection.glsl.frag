uniform sampler2D color1;
uniform sampler2D depth;

const float offset = 0.001;

varying vec2 texCoord;

void main(void)
{
    vec4 color = texture2D(color1, texCoord) * 8.0;
    color += texture2D(color1, texCoord + vec2(offset,0)) * -1.0;
    color += texture2D(color1, texCoord + vec2(-offset,0)) * -1.0;
    color += texture2D(color1, texCoord + vec2(0,offset)) * -1.0;
    color += texture2D(color1, texCoord + vec2(0,-offset)) * -1.0;

    color += texture2D(color1, texCoord + vec2(offset,offset)) * -1.0;
    color += texture2D(color1, texCoord + vec2(-offset,offset)) * -1.0;
    color += texture2D(color1, texCoord + vec2(offset,-offset)) * -1.0;
    color += texture2D(color1, texCoord + vec2(-offset,-offset)) * -1.0;

    gl_FragColor = texture2D(color1, texCoord) * pow(1.0 - length(color / 9.0), 12.0);
}
