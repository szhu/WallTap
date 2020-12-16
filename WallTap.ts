import { readLines } from "https://deno.land/std@0.77.0/io/bufio.ts";

let textEncoder = new TextEncoder();
let textDecoder = new TextDecoder();

function fixedLine(line: string) {
  return "\r\u001b[K" + line;
}

async function osascript(script: string) {
  let process = Deno.run({
    cmd: ["osascript", "-e", script],
    stdout: "null",
  });
  await process.status();
}

function serialProcess() {
  return Deno.run({
    cmd: ["bash", "-c", "cat /dev/cu.usbmodem*"],
    stdout: "piped",
  });
}

interface Event {
  type: "pressed" | "unpressed",
  times: number,
}

async function isScreenSaverRunning() {
  let process = Deno.run({ cmd: ["pgrep", "ScreenSaverEngine"] });
  let status = await process.status();
  return status.success;
}

async function toggleScreenSaver() {
  if (await isScreenSaverRunning()) {
    osascript(`quit app "ScreenSaverEngine"`);
  } else {
    Deno.run({ cmd: ["open", "-a", "ScreenSaverEngine"] });
  }
}

async function togglePlayback() {
  osascript(`
  beep
  tell app "Spotify" to playpause
`);
}


async function run() {
  console.log("Started!");

  let deferredSinglePress = false;

  let process = serialProcess();
  for await (let line of readLines(process.stdout)) {
    console.log(line);

    let event: Event = JSON.parse(line)

    if (event.type === "pressed") {
      if (event.times === 1) {
        deferredSinglePress = true
      }
      else if (event.times === 8) {
        console.log("=> toggleScreenSaver()");
        toggleScreenSaver()
        deferredSinglePress = false
      }
    }

    if (event.type === "unpressed") {
      if (deferredSinglePress) {
        console.log("=> togglePlayback()");
        togglePlayback()
        deferredSinglePress = false
      }

      console.log()
    }
  }
}


run();
