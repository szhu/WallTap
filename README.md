# WallTap

WallTap is a wired capacitive touch sensor that is easily mountable on any wall, or any other fixed surface. It has a huge surface area gives the user a very easy-to-reach control that can be programmed to do anything they want, and is something that they can activate without looking at a display -- or at anything at all!

<p align="center">
  <img
    alt="WallTap user in bed, hand pressed against the wall."
    src="docs/Screen%20Shot%202020-12-09%20at%208.59.33%20PM.png"
  />
  <br/>
  <a
    target="_blank"
    href="https://szhu.github.io/itp-blog/files/2020-09-pcomp/week-13/1_project_3/process/IMG_9117.opt.mp4"
  >⏯ Demo Video</a>
</p>

The current prototype must be attached to a computer and all touch actions must be initiated from the computer. The eventual goal would be for the device to be able flip switches, and to communicate with smart home devices. But even with the current limitations, WallTap has completely changed my bedtime experience, allowing me to control my music and turn off my "lights" without getting up, or worse, starting at a brightly-lit display.

WallTap has a secondary goal. The modern concept of smart home devices has become entangled with the concepts of luxurious pricing, wireless connectivity, and online accounts. With WallTap we attempt to buck these trends by creating "dumb smart home device" that is just as functional, without concerns of price, interference, or privacy.

## History

This was my final project for the Introduction to Physical Computing class at NYU ITP, Fall 2020. Build process blog posts:

- <https://szhu.github.io/itp-blog/posts/2020-11-25-pcomp.html>
- <https://szhu.github.io/itp-blog/posts/2020-12-09-pcomp.html>

## Usage

- **Circuit:** The circuit requires an Arduino plugged into the computer via USB. Wire the circuit according to the comment at the top of <WallTap.ino>. You may need to adjust the resistance based on your environment. The sensor is any conductive material, which the user will be touching to activate the sensor.

- **Arduino code:** Load the following code into the Arduino: `WallTap.ino`

- **Computer code:** This code currently only works on Mac, but the Mac-specific part is trivial. To use the code, install Deno (`brew install deno` or <https://deno.land/#installation>). The run `deno run --allow-run WallTap.ts`.

When you start the Arduino, it will spend a few seconds calibrating. (The serial monitor or the Deno program will show you when it's calibrating.) During the calbration period, you should move your hand to be close to the sensor, but not touching it. This will make sure that the system knows that that level of capacitance corresponds to not touching the sensor.

To re-calibrate, you can reset the Arduino by pressing the reset button on it.
