// The dimensions of hole in the top of the box into which
// the receipt printer fits.
hole_x = 72.7;
hole_y = 73.2;
hole_z = 50;
screw_post_face_depth = 34.6;
screw_post_od = 6.5;
screw_post_offset_left_x = 14 - screw_post_od/2;
screw_post_offset_right_x = 15 - screw_post_od/2;
screw_post_offset_y = 4.4+screw_post_od/2;
screw_hole_r = 1.5;

batt_holder_x = 21.0;
batt_holder_y = 75.7;
batt_holder_z = 20.0;

batt_r = 18/2;
batt_x = 65;

wt=2;
$fn = 32;

module hole() {
    cube([hole_x, hole_x, hole_x]);
    translate([screw_post_offset_left_x, screw_post_offset_y,-100]) cylinder(r=screw_hole_r, h=100);
    translate([hole_x-screw_post_offset_right_x, screw_post_offset_y,-100]) cylinder(r=screw_hole_r, h=100);
}

module housing() {
    difference () {
        cube([hole_x+2*wt, hole_x+2*wt, hole_x+wt]);
        translate([wt,wt,wt]) hole();
        // #translate([wt,wt,wt]) rotate([0,0,0]) batt_holder();
    }
    // batt();
}

module batt_holder() {
    cube([batt_holder_y, batt_holder_x, batt_holder_z]);
}
module batt() {
    rotate([-90,0,0]) cylinder(r=batt_r, h=batt_x);
}

// hole();

housing();
// batt_holder();