// LYWS 43 43 16
$fn=30;
difference() {
    minkowski() {
        cube([46,18,35],center=true);
        sphere(d=2);
    }
    difference() {
        translate([0,0,2.5])cube([44,16,39],center=true);
        translate([-17,7,0])cube([10,16-12,40],center=true);
        translate([17,7,0])cube([10,16-12,40],center=true);
    }
    translate([0,-5,4.5]) cube([28,16,28],center=true);
    translate([0,-5,12.5]) cube([28,16,28],center=true);
    translate([0,9,10])rotate([90,0,0])cylinder(h=2,d=3,center=true);
    translate([0,8.5,10])rotate([90,0,0])cylinder(h=1,d=6,center=true);
}