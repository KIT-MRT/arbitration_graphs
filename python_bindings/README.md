# Arbitration Graphs

[![Latest Release](https://img.shields.io/github/v/release/KIT-MRT/arbitration_graphs?color=green)](https://github.com/KIT-MRT/arbitration_graphs/releases)
[![License](https://img.shields.io/github/license/KIT-MRT/arbitration_graphs)](./LICENSE)
[![Unit Test Status](https://img.shields.io/github/actions/workflow/status/KIT-MRT/arbitration_graphs/run-unit-tests.yaml?branch=main&label=tests)](https://github.com/KIT-MRT/arbitration_graphs/actions/workflows/run-unit-tests.yaml?query=branch%3Amain)

*TL;DR: Replace those rusty state machines with safe and scalable arbitration graphs!*

**Hierarchical behavior models for complex decision-making and behavior generation in robotics!**

- 🌱 **Bottom-up**  
  Combine simple atomic behavior components to generate complex behaviors.
- 🧩 **Functional decomposition**  
  Behavior components address *How to do it?* and *Can we do it?*, while Arbitrators decide *What to do?*
 - 🧠 **Meta-framework**  
  Integrate diverse methods in one decision-making framework. Why not combine optimization-based planning, probabilistic approaches (POMDPs), and machine learning (RL)? Use any approach where it performs best!
- 🛠️ **Maintainability**  
  Add new behaviors without having to touch existing ones – did we mention strict modularity and functional decomposition?
- 🛡️ **Behavior Verification**  
  Use tightly integrated verifiers to ensure that only safe and valid behavior commands are executed.
- 🪂 **Graceful Degradation**  
  Your behavior is unreliable or unsafe? Arbitrators will gracefully fall back to the next-best option.


<details>
<summary>😋 Click for more reasons!</summary>

- 📈 **Scalability**  
  Stack behavior components in arbitrators to create hierarchical behavior models.
- 💡 **Transparency**  
  Easily follow and understand the decision-making process, e.g., with our GUI.
- 📦 **Header-Only**  
  Simple integration – just include this header-only C++17 library!
- 🐍 **Python Bindings**  
  For easy prototyping, testing, and integration of machine learning algorithms, all the functionality is available via Python bindings.
- 📜 **Permissive License**  
  Published under MIT license to ensure maximum flexibility for your projects.

</details>


<details>
<summary>🤨 How does it compare to Behavior Trees?</summary>

Behavior Trees (BTs) are great for a variety of applications and thrive within a vibrant community!  
Kudos to [Petter Ögren's](https://www.kth.se/profile/petter/) crew, [Michele Colledanchise](https://miccol.github.io/behaviortrees/) and [Davide Faconti](https://github.com/facontidavide) 🖖

But, Arbitration Graphs bring great value, especially for safety critical applications like self-driving cars and mobile robots in general – by strictly coupling preconditions to behaviors and tightly integrating behavior verification.
A bit more in detail:

|                   | Behavior Trees                                                                                                                 | Arbitration Graphs                                                                                                                                                                                                    |
| ----------------- | ------------------------------------------------------------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Interfaces**    | Nodes return execution status (*success*, *failure*, or *running*). <br>⏵ more flexibility w.r.t. a node's actuator interfaces | Behavior components & arbitrators return commands (e.g., a trajectory). <br>⏵ control theory motivated interface ${f(\boldsymbol{x}) \to \boldsymbol{u}}$ <br>⏵ command can be verified by each arbitrator            |
| **Preconditions** | Implemented by condition nodes distributed throughout the tree. <br>⏵ easy to reuse preconditions for multiple behaviors       | Require behavior components to define their own preconditions. <br>⏵ tight coupling of preconditions to behaviors <br>⏵ robustness and safety less dependent on node arrangement                                      |
| **Safety**        | Each node decides on its success or failure. <br>⏵ can lead to safety and reliability issues, if not carefully managed         | Integrate safety into the selection mechanism, using node-independent verifiers. <br>⏵ reduces the burden on behavior engineers <br>⏵ allows an easy integration of unsafe behavior components (ML, probabilistic, …) |

</details>

## At a Glance

**Arbitration Graphs** break down complex decision-making into atomic behavior components and arbitrators.

**Behavior components** compute a command (e.g., a trajectory) based on the current state of the world.
They define whether they can be executed in the current state using their invocation condition.

**Arbitrators** select the best option based on a defined decision-making policy.
Options can be behavior components or nested arbitrators.

**Verifiers** check whether the command of a behavior component is safe and valid prior to being selected by an arbitrator.

<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/arbitration_graphs_at_a_glance_dark.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/arbitration_graphs_at_a_glance_light.svg">
    <img src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/arbitration_graphs_at_a_glance_light.svg">
  </picture>
</p>


## Demo

We provide a demo of this library using Pac-Man as an example application.  
The arbitration graph controls Pac-Man on its journey to collect tasty dots 🍬

Run the demo with:

```bash
git clone https://github.com/KIT-MRT/arbitration_graphs.git
cd arbitration_graphs/demo
docker compose up
```

Open the GUI with your favorite browser:  
[http://0.0.0.0:8080](http://0.0.0.0:8080)

### Explanation

You will see the *Pacman Agent* arbitrator selecting between five behavior options (by priority).  
The *Eat Dots* option is itself an arbitrator with two sub-behaviors (selecting by expected benefit).

In this scene,
- the *Chase Ghost* and *Avoid Ghost* behaviors are not applicable (no ghosts in close vicinity) → grey background,
- the *Eat Closest Dot* and *Move Randomly* behavior failed verification (our verification showcase) → red background,
- thus, the least-prioritized *Stay in Place* behavior is being executed → green background.

<p align="center">
  <img src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/pacman_scenario_cropped.png" width="300" />
</p>
<p align="center">
  <img src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/pacman_arbitrator_safe.svg" width="500" /> 
</p>

## Installation

> **Note:** We are currently working on a new major release of the library which will include some breaking changes.
> See the corresponding [issue](https://github.com/KIT-MRT/arbitration_graphs/issues/116) and [pull request](https://github.com/KIT-MRT/arbitration_graphs/pull/123) for details and the current status.

The Python interface to the arbitration graph library is generated using `pybind11` and provides a convenient interface to utilize the full power of the arbitration graph library in Python.

<details>
<summary>Prerequisites</summary>

First make sure all dependencies are installed:
- The arbitration graph core library (see [../README.md](../README.md))
- Python 3.8 or higher
- [pybind11](https://pybind11.readthedocs.io/en/stable/installing.html)
- [ninja](https://ninja-build.org/)

See also the [`Dockerfile`](./Dockerfile) for how to install these packages under Debian or Ubuntu.
</details>

<details>
<summary>From source via pip</summary>


```bash
# From local repository
git clone https://github.com/KIT-MRT/arbitration_graphs.git
cd arbitration_graphs/python_bindings
pip install .

# Or directly from GitHub
pip install git+https://github.com/KIT-MRT/arbitration_graphs.git#subdirectory=python_bindings
```
</details>

<details>
<summary>From source via CMake</summary>

Clone the repository and build the package using CMake:
```bash
mkdir -p arbitration_graphs/python_bindings/build
cd arbitration_graphs/python_bindings/build
cmake ..
cmake --build .
```
</details>

## Development

<details>
<summary>Using Docker image</summary>

Clone the repository and run the development image

```bash
git clone https://github.com/KIT-MRT/arbitration_graphs.git
cd arbitration_graphs/python_bindings
docker compose build arbitration_graphs_pybind_devel
docker compose run --rm arbitration_graphs_pybind_devel
```

This mounts the source into the container's `/home/blinky/arbitration_graphs` folder.
There, you can edit the source code, compile and run the tests etc.

</details>


<details>
<summary>Running unit tests</summary>

This package includes unit tests analogous to the C++ tests.
To run all tests, use the following command:

```bash
cd arbitration_graphs/python_bindings/test
python -m unittest discover
```
</details>

## Contributors

This library and repo has been crafted with ❤️ by

**Christoph Burger**
<span>&nbsp;
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark-white.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark.svg">
    <img height="16" class="github-logo" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark.svg" alt="GitHub Invertocat"/>
  </picture>
  <a href="https://github.com/ChristophBurger89" aria-label="View GitHub profile">
    ChristophBurger89
  </a>
  &nbsp;
  <img style="height:1em; position: relative; top:.2em" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/In-Blue-128@2x.png" alt="LinkedIn logo"/>
  <a href="https://www.linkedin.com/in/christoph-burger" aria-label="View LinkedIn profile">
    christoph-burger
  </a>
  &nbsp;
  <img style="height:1em; position: relative; top:.2em" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/ORCID-iD_icon_vector.svg" alt="ORCID iD"/>
  <a href="https://orcid.org/0009-0002-9147-8749" aria-label="View ORCID record">
    0009-0002-9147-8749
  </a>
</span>
<br>
**Nick Le Large**
<span>
  &nbsp;
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark-white.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark.svg">
    <img height="16" class="github-logo" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark.svg" alt="GitHub Invertocat"/>
  </picture>
  <a href="https://github.com/ll-nick" aria-label="View GitHub profile">
    ll-nick
  </a>
  &nbsp;
  <img style="height:1em; position: relative; top:.2em" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/In-Blue-128@2x.png" alt="LinkedIn logo"/>
  <a href="https://www.linkedin.com/in/nick-le-large" aria-label="View LinkedIn profile">
    nick-le-large
  </a>
  &nbsp;
  <img style="height:1em; position: relative; top:.2em" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/ORCID-iD_icon_vector.svg" alt="ORCID iD"/>
  <a href="https://orcid.org/0009-0006-5191-9043" aria-label="View ORCID record">
    0009-0006-5191-9043
  </a>
</span>
<br>
**Piotr Spieker**
<span>
&nbsp;
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark-white.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark.svg">
    <img height="16" class="github-logo" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark.svg" alt="GitHub Invertocat"/>
  </picture>
  <a href="https://github.com/orzechow" aria-label="View GitHub profile">
    orzechow
  </a>
  &nbsp;
  <img style="height:1em; position: relative; top:.2em" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/In-Blue-128@2x.png" alt="LinkedIn logo"/>
  <a href="https://www.linkedin.com/in/piotr-spieker" aria-label="View LinkedIn profile">
    piotr-spieker
  </a>
  &nbsp;
  <img style="height:1em; position: relative; top:.2em" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/ORCID-iD_icon_vector.svg" alt="ORCID iD"/>
  <a href="https://orcid.org/0000-0002-0449-3741" aria-label="View ORCID record">
    0000-0002-0449-3741
  </a>
</span>

Christoph and Piotr coded the core in a pair-programming session.
Piotr also contributed the GUI and GitHub Page.
Nick implemented the awesome PacMan demo and tutorial, with drafting support by Christoph, reviews and finetuning by Piotr.
The Python bindings have been contributed by Nick and reviewed by Piotr.

The repository is maintained by Piotr Spieker&nbsp;
<span>
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark-white.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark.svg">
    <img height="16" class="github-logo" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark.svg" alt="GitHub Invertocat"/>
  </picture>
  <a href="https://github.com/orzechow" aria-label="View GitHub profile">
    orzechow
  </a>
</span>
and
Nick Le Large&nbsp;
<span>
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark-white.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark.svg">
    <img height="16" class="github-logo" src="https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets/img/github-mark.svg" alt="GitHub Invertocat"/>
  </picture>
  <a href="https://github.com/ll-nick" aria-label="View GitHub profile">
    ll-nick
  </a>
</span>.


## Citation

If you use arbitration graphs in your research, we would be pleased if you cite our work:

*Piotr Spieker, Nick Le Large, and Martin Lauer, “Better Safe Than Sorry: Enhancing Arbitration Graphs for Safe and Robust Autonomous Decision-Making,” Nov. 15, 2024, arXiv: arXiv:2411.10170. doi: [10.48550/arXiv.2411.10170](https://doi.org/10.48550/arXiv.2411.10170).*

```bibtex
@misc{spieker2024ArbitrationGraphs,
  title={Better Safe Than Sorry: Enhancing Arbitration Graphs for Safe and Robust Autonomous Decision-Making}, 
  author={Piotr Spieker and Nick Le Large and Martin Lauer},
  year={2024},
  eprint={2411.10170},
  eprinttype = {arXiv},
  archivePrefix={arXiv},
  primaryClass={cs.RO},
  doi = {10.48550/arXiv.2411.10170},
  url={https://arxiv.org/abs/2411.10170}, 
}
```


<details>
<summary>Earlier publications</summary>

### Behavior Verification and Fallback Layers
A safety concept that extends Arbitration Graphs with behavior verification and fallback layers in the context of automated driving has been proposed by Piotr Spieker (née Orzechowski) in his PhD thesis.
This served as the basis for the paper with Nick above.

_Piotr F. Orzechowski, “Verhaltensentscheidung für automatisierte Fahrzeuge mittels Arbitrationsgraphen,” phd, Karlsruher Institut für Technologie (KIT), 2023. doi: [10.5445/IR/1000160638](https://doi.org/10.5445/IR/1000160638)._

```bibtex
@thesis{Orzechowski2023Arbitrationsgraphen,
  type = {phdthesis},
  title = {Verhaltensentscheidung für automatisierte Fahrzeuge mittels Arbitrationsgraphen},
  author = {Orzechowski, Piotr Franciszek},
  date = {2023},
  institution = {Karlsruher Institut für Technologie (KIT)},
  doi = {10.5445/IR/1000160638},
  langid = {german},
  pagetotal = {169},
}
```

### Replacing state machines in AV
Arbitration Graphs replaced state machines in the context of automated driving at the Institute of Measurement and Control Systems (MRT) of the Karlsruhe Institute of Technology (KIT):

_Piotr F. Orzechowski, Christoph Burger, and Martin Lauer, “Decision-Making for Automated Vehicles Using a Hierarchical Behavior-Based Arbitration Scheme,” in Intelligent Vehicles Symposium, Las Vegas, NV, USA: IEEE, Oct. 2020, pp. 767–774. doi: [10.1109/IV47402.2020.9304723](https://doi.org/10.1109/IV47402.2020.9304723)._

```bibtex
@inproceedings{orzechowski2020ArbitrationGraphs,
  title = {Decision-Making for Automated Vehicles Using a Hierarchical Behavior-Based Arbitration Scheme},
  booktitle = {Intelligent Vehicles Symposium},
  author = {Orzechowski, Piotr F. and Burger, Christoph and Lauer, Martin},
  date = {2020-10},
  pages = {767--774},
  publisher = {IEEE},
  location = {Las Vegas, NV, USA},
  issn = {2642-7214},
  doi = {10.1109/IV47402.2020.9304723},
}
```

### Foundation work in Robot Soccer
The foundations for Arbitration Graphs have been proposed in the context of robot soccer:

_Martin Lauer, Roland Hafner, Sascha Lange, and Martin Riedmiller, “Cognitive concepts in autonomous soccer playing robots,” Cognitive Systems Research, vol. 11, no. 3, pp. 287–309, 2010, doi: [10.1016/j.cogsys.2009.12.003](https://doi.org/10.1016/j.cogsys.2009.12.003)._

```bibtex
@article{lauer2010CognitiveConceptsAutonomous,
  title = {Cognitive Concepts in Autonomous Soccer Playing Robots},
  author = {Lauer, Martin and Hafner, Roland and Lange, Sascha and Riedmiller, Martin},
  date = {2010},
  journaltitle = {Cognitive Systems Research},
  volume = {11},
  number = {3},
  pages = {287--309},
  doi = {10.1016/j.cogsys.2009.12.003},
}
```

</details>
