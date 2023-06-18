#version 330 core

// Ouput data
out vec4 color;


void main()
{ 
  int gridStep = 20;
  vec2 step = fract(gl_FragCoord.xy / gridStep); 
  if((step.x > 0.5 && step.y > 0.5)||
		(step.x <= 0.5 && step.y <= 0.5)) {
		color = vec4(0.2,0.2,0.2,1);
  }
  else {color = vec4(0.3,0.3,0.3,1);}
}