#version 330

in vec3 Color;

uniform vec3 SolidColor = vec3(-1,-1,-1);
uniform bool Complementary = false;

out vec4 Fragment;

main(){
    if(Complementary){
        Fragment = vec4(1 - Color.r, 1 - Color.g, 1 - Color.b, 1);
        
        if(SolidColor.r != -1 && SolidColor.g != -1 && SolidColor.b != -1){
            Fragment = vec4(1 - SolidColor, 1);
        }
    }
    else{
        Fragment = vec4(Color, 1);
        
        if(SolidColor.r != -1 && SolidColor.g != -1 && SolidColor.b != -1){
            Fragment = vec4(SolidColor, 1);
        }
    }
    
    return;
}