// max height 12.5mm
// height without cover 11.2mm
// battery hole with raised battery 1.4mm
// battery hole with battery down: 2mm
// total circle+triangle module length 23mm
translate ([-20/3+10,0,0]) difference() {
    cylinder(d=20,h=0.5,center=true);
    translate([20/3,0,0])cube([20,20,0.5],center=true);
}
translate([(23-(20/3))/2,0,0])cube([23-(20/3),10,0.5],center=true);
translate([-20/3+10,0,2.7/2]) difference() {
    cube([10,10,2.7],center=true);
    cube([6.3,6.3,3],center=true);
    translate([0,2.5,0])cube([10,1,3],center=true);
    translate([0,-2.5,0])cube([10,1,3],center=true);
}