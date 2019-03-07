 width = 64;
length = 130;
depth = 15;
wall = 1;
fitting = 0.001;

display_width = 41;
display_length = 99;
snug_fit = 0.3;
union(){
    difference(){
        // case
        cube([width, length, depth]);
        // case cutout
        translate([wall, wall,wall])
        cube([width-2*wall, length-2*wall, depth-wall+fitting]);    
        // display cutout
        translate([(width-display_width)/2,3,-fitting])
        cube([display_width, display_length, wall+2*fitting]);
        // button cutout
        translate([width/2,length - 12,0])
        cylinder(wall*2+fitting, 3.5, 3.5, true);
        // case cutout
        translate([width-wall-fitting, length - 9 - 10,depth-10])
        cube([wall+2*fitting,9,4]);
    }

    
    // lid
    translate([width+20,0,0])
    cube([width - 2*wall - snug_fit, length- 2*wall - snug_fit, wall]);
}