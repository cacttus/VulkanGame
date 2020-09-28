
function toNumber(number, fallback) {
  print("got here");
  var i = parseFloat(number);
  print("got here");
  return isNaN(i) ? fallback : i;
}
function random(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}
function Vec2(xx, yy) {
  print("got here");
  this.x = parseFloat(xx, 0);
  this.y = parseFloat(yy, 0);
}
Vec2.multiply = function (rhs) {
  return new Vec2(this.x * rhs.x, this.y * rhs.y);
};

print("hello world tinyjs");

var dooga = function (x) {
  print("  hi again?" + x);
};

for (var n = 0; n < 10; n++) {
  print("Hi " + n);
  if (n == 4) {
    dooga(n);
  }
}

obj1 = new Object();
obj1.food = "cake";
obj1.desert = "pie";
obj2 = obj1.clone();
obj2.food = "kittens";

var vv0 = Object.create(Vec2, { x:0, y:0 } );
//var v1 = new Vec2(3, 4);

//print("v0 = (" + v0.x.toString() + "," + v0.y.toString() + ")");

dump();
//print("v1 = " + v1.toString());
//
//var v2 = v0.multiply(v1);
//
//print("v2 = " + v2.toString());