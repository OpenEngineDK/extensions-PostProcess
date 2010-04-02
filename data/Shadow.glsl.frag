uniform sampler2D color1;
uniform sampler2D depth;

const vec3 screenLightPos = vec3(0.0, 0.0, 0.0); // light pos i screenspace (som global coords, men (0,0) er i øverst venstre hjørne (1,1) nederst højre)

varying vec2 texCoord;

float convertDepthToZ(float depth);

void main() {

     float NUM_SAMPLES = 100.0;

    //vec3 screenLightPos = vec3(0.5, 0.5, -2.0);


     vec2 texcoord = texCoord;
       
    // get the color
    vec4 color = texture2D(color1, texcoord);
    
    // calculate vector from pixel to light source in screen space.
    vec2 light2pixelVector = texcoord - screenLightPos.xy;
    //vec2 light2pixelVector = texcoord - ((screenLightPos.xy / -screenLightPos.z) * 2 + vec2(-1,-1));
    
    // divide by number of samples
    vec2 deltaTexcoord =  light2pixelVector / NUM_SAMPLES;
    
    // store initial sample.
    float z = convertDepthToZ(texture2D(depth, texcoord).x);
    
    // remember the z values at the pixel and at the light (so we have a z-line)
    float zlight = -screenLightPos.z; //texture2D(depthbuf, texcoord - light2pixelVector).x;
    float zpixel = z;
    
    // the amount the z changes on the line for each sample
    float deltaz = (zlight - zpixel) / NUM_SAMPLES; // OMVENDT???

    float zline = zpixel;

    // sample along line in screen space (starts at pixelpos and ends at light pos)
    for (float i = 0.0; i < NUM_SAMPLES; i++) {
    
        // step sample location along line
        texcoord -= deltaTexcoord;
        zline += deltaz;
        
        // retrieve sample at new location
        
        z = convertDepthToZ(texture2D(depth, texcoord).x);
        
        // see if this sampled z is closer to the eye than the z of the line at this point - if if z < zline. If so the pixel is occluded.
//        float zline = zpixel + deltaz * float(i+1);

        //if (z < zline) color = vec4(0,0,0,1);                         
        if (z < zline) color = color * 0.95;
        //if (length(gl_TexCoord[0].xy - texcoord) > 0.1) if (z < zline) color = color * 0.95;
        //if (z < zline) color = color * 0.95 + texture2D(colorbuf, texcoord)*0.05;
    }

    //if (length(gl_TexCoord[0].xy - ((screenLightPos.xy / -screenLightPos.z) * 2 + vec2(-1,-1))) < 0.01) color = vec4(1,1,1,1);
    if (length(gl_TexCoord[0].xy - screenLightPos.xy) < 0.01) color = vec4(1,1,1,1);

    gl_FragColor = color;

}

float convertDepthToZ(float depth) {
    float znear = 1.0;
    float zfar  = 50.0;
    float z = znear*zfar / (zfar - depth*(zfar-znear));
    return z;
}
