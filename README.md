# IFCC - Compilateur pour un sous-ensemble de C

## Table des matières

- [Introduction](#introduction)
- [Utilisation](#utilisation)
  - [Compilation du compilateur IFCC](#compilation-du-compilateur-ifcc)
  - [Compilation d'un programme](#compilation-dun-programme)
  - [Assemblage et linking](#assemblage-et-linking)
  - [Exécution du programme](#exécution-du-programme)
- [Tests](#tests)

## Introduction

IFCC est un compilateur développé pour traduire du code source écrit dans un sous-ensemble de C en code assembleur Intel x86 ou RISC-V.

## Utilisation

### Compilation du compilateur IFCC

Pour compiler IFCC, suivez ces étapes :

1. Assurez-vous que vous avez `make` installé sur votre système.
3. Allez dans le répertoire racine du projet IFCC.
4. Lancez la commande `make`.

### Compilation d'un programme

Une fois IFCC compilé, vous pouvez utiliser le compilateur pour traduire votre programme écrit dans le sous-ensemble de C pris en charge. Voici comment procéder :

1. Écrivez votre programme dans un fichier avec l'extension `.c`.
2. Utilisez la commande suivante pour compiler votre programme :

```bash
ifcc <nom_fichier_source>.c [-t target] [-o <nom_fichier_cible>.s]
```

- L'option `-t target` vous permet de spécifier l'architecture cible pour la génération de l'assembleur. Vous pouvez choisir entre `x86-64` pour Intel x86 et `rv64` pour RISC-V. Par défaut, le code est compilé en assembleur Intel x86.
- L’option `-o <nom_fichier_cible>.s` permet de spécifier le nom du fichier de sortie pour le code assembleur généré. Par défaut, le nom du fichier de sortie est `./out.s`.

### Assemblage et linking

Après avoir généré le code assembleur, vous devez l'assembler et le linker en fonction de l'architecture cible. Selon votre machine, vous pouvez par exemple procéder comme suit :

- Pour assembler et linker le code assembleur en code exécutable Intel x86 :

```bash
gcc <nom_fichier_source>.s [-o <nom_executable>]
```

- Pour assembler et linker le code assembleur en code exécutable RISC-V :

```bash
riscv64-linux-gnu-gcc <nom_fichier_source>.s [-o <nom_executable>]
```

Par défaut, le nom de l’exécutable généré est `./a.out`.

### Exécution du programme

Une fois que vous avez obtenu l'exécutable, vous pouvez l'exécuter en utilisant la commande suivante :

```bash
./a.out
```

Si vous avez compilé en RISC-V et souhaitez exécuter le programme sur un émulateur, veuillez utiliser l'émulateur approprié pour l'architecture RISC-V.

## Tests

IFCC est livré avec un ensemble de tests d'intégration pour vérifier son bon fonctionnement. Les tests se trouvent dans le dossier `tests/testfiles/`. Vous pouvez exécuter tous les tests en utilisant la commande suivante :

```bash
$ make check
```

Cela lancera les tests avec l'architecture Intel x86 et RISC-V. Vous pouvez également spécifier `make check-x86` ou `make check-rv64` pour exécuter les tests sur une architecture spécifique.