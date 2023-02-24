let tether = 0;

export function tick() {
  tether++;

  // return {
  //   swag: "wicked",
  //   tether,
  // };
  return tether;
}

export function tick2() {
  tether++;

  return `loop ${tether}`;
}

export function tick3() {
  tether++;

  return `loop ${tether}`;
}

Deno.core.print("swag\n\n");

// return "awa";
