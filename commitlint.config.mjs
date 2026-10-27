import { config } from "./tooling/commitlint.mjs";

export default config([
  "tooling", // tooling/: release scripts, configs, commitlint
  "themes", // themes/
  "cpp", // the greylock:: library
  "qml", // Greylock.Ui
  "android", // android/: the Compose library
  "cli", // command-line helpers
  "www", // shared site files
  "tests",
  "ci",
  "deps",
  "deps-dev", // Dependabot's updates to development dependencies
]);
