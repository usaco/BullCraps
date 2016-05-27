BullCraps
===========

BullCraps! USACO 2016

## Have questions?

~~Try the [FAQ/Wiki](http://github.com/usaco/BullCraps/wiki).~~

## Need updates?

If you need to update your local copy, navigate to `BullCraps/` and try:

```bash
git pull origin master
```

## What is BullCraps?

TODO

## How do I get started?

Begin by cloning a copy of the Git repo:

```bash
git clone git://github.com/usaco/BullCraps.git
```

Go to the `base/` directory and compile the driver:

```bash
cd BullCraps/base
make
```

This will create the BullCraps driver.

Next, build the example bots to help with testing:

```bash
cd ../bots
bash build-all.sh
```

Make a new bot to begin working. A helper script has been provided for this purpose.

```bash
cd ..
bash setup-bot.sh "My Awesome Bot"
```

This will create a new directory `MyAwesomeBot/` in the directory `bots/`.
