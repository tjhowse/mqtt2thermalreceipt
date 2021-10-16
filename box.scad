wt=2;

// The dimensions of hole in the top of the box into which
// the receipt printer fits.
hole_x = 72.7;
hole_y = 73.2;
hole_z = 42;
screw_post_face_depth = 34.6;
screw_post_od = 6.5;
screw_post_offset_left_x = 15 - screw_post_od/2;
screw_post_offset_right_x = 14 - screw_post_od/2;
screw_post_offset_y = 4.4+screw_post_od/2;
screw_hole_r = 2;

batt_holder_x = 21.0;
batt_holder_y = 75.7;
batt_holder_z = 20.0;

batt_r = 18/2;
batt_x = 65;

button_r = 2.5;
button_edge_offset = 10;
button_spacing = 30;

box_x = batt_holder_y  + wt*2;
box_y = hole_y + batt_holder_x + wt*2;
box_z = hole_z+wt*2;

$fn = 32;

module hole() {
    cube([hole_x, hole_x, hole_x]);
    translate([screw_post_offset_left_x, screw_post_offset_y,-100]) cylinder(r=screw_hole_r, h=100);
    translate([hole_x-screw_post_offset_right_x, screw_post_offset_y,-100]) cylinder(r=screw_hole_r, h=100);
}

module housing() {
    difference () {
        cube([box_x, box_y, box_z]);
        translate([wt,wt,wt]) cube([box_x-wt*2, box_y-wt*2, box_z-wt*2]);
        translate([wt+(batt_holder_y-hole_x)/2,wt,wt]) hole();
        translate([box_x-wt-button_r-button_edge_offset,hole_y+batt_holder_x/2+wt,wt]) cylinder(r=button_r, h=100);
        translate([box_x-wt-button_r-button_edge_offset-button_spacing,hole_y+batt_holder_x/2+wt,wt]) cylinder(r=button_r, h=100);
    }
}

module lid() {
    gap = 0.25;
    intersection() {
        housing();
        cube([200,200,wt]);
    }
    difference() {
        translate([wt+gap,wt+gap,wt]) cube([box_x-2*(wt+gap), box_y-2*(wt+gap),wt]);
        translate([(wt+gap)*2,(wt+gap)*2,wt]) cube([box_x-4*(wt+gap), box_y-4*(wt+gap),wt]);
    }
}

module top() {
    difference() {
        housing();
        cube([200,200,wt]);
    }
}

module batt_holder() {
    cube([batt_holder_y, batt_holder_x, batt_holder_z]);
}
module batt() {
    rotate([-90,0,0]) cylinder(r=batt_r, h=batt_x);
}

// hole();

// housing();
// render() lid();
rotate([180,0,0]) render() top();
// batt_holder();