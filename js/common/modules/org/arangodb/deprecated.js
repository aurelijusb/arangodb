'use strict';
const semver = require('semver');
const db = require('org/arangodb').db;

function deprecated(version, graceReleases, message) {
  if (typeof version === 'number') {
    version = String(version);
  }
  if (typeof version === 'string' && !semver.valid(version)) {
    if (semver.valid(version + '.0.0')) {
      version += '.0.0';
    } else if (semver.valid(version + '.0')) {
      version += '.0';
    }
  }
  const arangoVersion = db._version();
  const arangoMajor = semver.major(arangoVersion);
  const arangoMinor = semver.minor(arangoVersion);
  const deprecateMajor = semver.major(version);
  const deprecateMinor = semver.minor(version);
  if (!message && typeof graceReleases === 'string') {
    message = graceReleases;
    graceReleases = 1;
  }
  if (!message) {
    message = 'This feature is deprecated.';
  }
  if (arangoMajor >= deprecateMajor) {
    if (arangoMajor > deprecateMajor || arangoMinor >= deprecateMinor) {
      throw new Error(`DEPRECATED: ${message}`);
    }
    if (arangoMinor >= (deprecateMinor - graceReleases)) {
      console.warn(`DEPRECATED: ${message}`);
    }
  }
}

module.exports = deprecated;
