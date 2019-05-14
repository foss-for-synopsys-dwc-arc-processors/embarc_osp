# Contributing to embARC OSP

Thank you for contributing to embARC.

The following is a set of guidelines for contributing to embARC Open Software Platform (OSP), which is hosted in the [embARC Open Software Platform](https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp) repository on GitHub.

These guidelines must be followed when submitting contributions to Synopsys projects to help ensure requests are processed as efficiently as possible. If you have any questions or concerns, please feel free to contact us at [opensource@synopsys.com](mailto:opensource@synopsys.com).

#### Table Of Contents

[What should I know before I get started?](#what-should-i-know-before-i-get-started)
  * [Review Code of Conduct](#code-of-conduct)
  * [Create GitHub account and sign Contribution Agreement](#create-github-account-and-sign-contribution-agreement)
  * [embARC OSP and Applications](#embarc-osp-and-applications)
  * [embARC OSP Design Decisions](#embarc-osp-design-decisions)

[How Can I Contribute?](#how-can-i-contribute)
  * [Reporting Bugs](#reporting-bugs)
  * [Suggesting Enhancements](#suggesting-enhancements)
  * [Your First Code Contribution](#your-first-code-contribution)
  * [Pull Requests](#pull-requests)

[Styleguides](#styleguides)
  * [Git Commit Messages](#git-commit-messages)
  * [C Styleguide](#c-styleguide)
  * [Documentation Styleguide](#documentation-styleguide)

[Additional Notes](#additional-notes)
  * [Issue and Pull Request Labels](#issue-and-pull-request-labels)

## What should I know before I get started?

### Code of Conduct

This project adheres to the Contributor Covenant [code of conduct](./CODE_OF_CONDUCT.md).
By participating, you are expected to uphold this code.
Please report unacceptable behavior to [info@embarc.org](mailto:info@embarc.org).

### Create GitHub account and sign Contribution Agreement

Make sure you have a [GitHub account](https://github.com/join). If your contribution was developed during employment or while contracting for a third-party, ensure that such owner or contracting party has provided explicit approval for your contribution to the Synopsys project under the terms of the Synopsys Contributor License Agreement.

Submit the [Synopsys Contributor License Agreement](https://www.clahub.com/agreements/foss-for-synopsys-dwc-arc-processors/embarc_osp) on CLAhub.com.

### embARC OSP and Applications

embARC OSP is an open software platform designed to help accelerate the development and production of embedded systems based on DesignWare ARC processors. Applications are the applications which are writing based on embARC Open Software Platform.

We host the following two repositories to be used in conjunction with each other when working with embARC OSP:
- [embarc_osp](https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp) - The embARC Open Software Platform including core services, platform support adn integrated networking, security and IoT middleware
- [embarc_applications](https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_applications) - Applications written using the embARC OSP

When you consider contributing or raising a issue against embARC OSP or Applications, please go to corresponding repository above.

### embARC OSP Design Decisions

When we make a significant decision in how we maintain the project and what we can or cannot support, we will document it in the github wiki. If you have a question around how we do things, check to see if it is documented there. If it is *not* documented there, please open a new issue on [embARC Open Software Platform](https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp/issues) and ask your question.

## How Can I Contribute?

### Reporting Bugs

This section guides you through submitting a bug report against embARC projects. Following these guidelines helps maintainers and the community understand your report :pencil:, reproduce the behavior :computer: :computer:, and find related reports :mag_right:.

Before creating bug reports, please check [this list](#before-submitting-a-bug-report) as you might find out that you don't need to create one. When you are creating a bug report, please [include as many details as possible](#how-do-i-submit-a-good-bug-report). Please use the template provided for this purpose.

#### Before Submitting A Bug Report

* **Check the embARC OSP documentation.** You might be able to find the cause of the problem and fix things yourself. Most importantly, check if you can reproduce the problem in the latest version of embARC OSP, if the problem happens when you run using both the Metaware and the GNU toolchain for ARC Processors.
* **Check the embARC OSP FAQs** for a list of common questions and problems.
* **Determine [which repository the problem should be reported in](#embarc-osp-and-applications)**.
* **Perform a cursory search in related embARC repositories** to see if the problem has already been reported. If it has, add a comment to the existing issue instead of opening a new one.

#### How Do I Submit A (Good) Bug Report?

Bugs are tracked as [GitHub issues](https://guides.github.com/features/issues/). After you've determined [which repository](#embarc-osp-and-applications) your bug is related to, create an issue on that repository and provide the following information.

Explain the problem and include additional details to help maintainers reproduce the problem:

* **Use a clear and descriptive title** for the issue to identify the problem.
* **Describe the exact steps which reproduce the problem** in as many details as possible. For example, start by explaining which release or commit of embARC OSP you are using, and how you use embARC OSP, e.g. which command exactly you used in the terminal. When listing steps, **don't just say what you did, but explain how you did it**. For example, if you run a command, explain which terminal you use and the directory where you run the command?
* **Provide specific examples to demonstrate the steps**. Include links to files or GitHub projects, or copy/pasteable snippets, which you use in those examples. If you're providing snippets in the issue, use [Markdown code blocks](https://help.github.com/en/articles/getting-started-with-writing-and-formatting-on-github).
* **Describe the behavior you observed after following the steps** and point out the exact problem with that behavior.
* **Explain which behavior you expected to see instead and why.**
* **Include screenshots and animated GIFs** which show you following the described steps and clearly demonstrate the problem. You can try to **record the GIF of the steps to reproduce the bug**. You can use [licecap](https://www.cockos.com/licecap/) to record GIFs on macOS and Windows, and [silentcast](https://github.com/colinkeenan/silentcast) on Linux.
* **If you're reporting embARC OSP compilation failures**, include a verbose compiling log. Include the compiling log in the issue in a [code block](https://help.github.com/en/articles/getting-started-with-writing-and-formatting-on-github), a [file attachment](https://help.github.com/en/articles/file-attachments-on-issues-and-pull-requests), or put it in a [gist](https://gist.github.com/discover) and provide link to that gist.
* **If the problem happens randomly**, describe what you were doing before the problem happened and share more information using the guidelines below.

Provide more context by answering these questions:

* **Can you reproduce the problem in latest release of embARC OSP?**
* **Can you reliably reproduce the issue?** If not, provide details about how often the problem happens and under which conditions it normally happens.
* If the problem is related to embARC applications, please go to the embARC applications repository and report is by filing an issue

Include details about your configuration and environment:

* **Which version of embARC OSP are you using?** Provide the release version or commit of embARC OSP.
* **Provide name and version of the OS and toolchain you are using**
* **Provide hardware board and arc core you are using**

#### [Template For Submitting Bug Reports](./ISSUE_TEMPLATE.md)

### Suggesting Enhancements

This section guides you through submitting an enhancement suggestion for embARC OSP, including completely new features and minor improvements to existing functionality. Following these guidelines helps maintainers and the community understand your suggestion :pencil: and find related suggestions :mag_right:.

Before creating enhancement suggestions, please check [this list](#before-submitting-an-enhancement-suggestion) as you might find out that you don't need to create one. When you are creating an enhancement suggestion, please [include as many details as possible](#how-do-i-submit-a-good-enhancement-suggestion). If you'd like, you can use [this template](#template-for-submitting-enhancement-suggestions) to structure the information.

#### Before Submitting An Enhancement Suggestion

* Check the embARC OSP documentation, and embARC OSP roadmap.
* Check if the enhancement is already developed or in development in latest sourcecode.
* **Determine [which repository the enhancement should be suggested in](#embarc-osp-and-applications).**
 **Perform a cursory search in related embARC repositories** to see if the enhancement has already been suggested. If it has, add a comment to the existing issue instead of opening a new one.

#### How Do I Submit A (Good) Enhancement Suggestion?

Enhancement suggestions are tracked as [GitHub issues](https://guides.github.com/features/issues/). After you've determined [which repository](#embarc-osp-and-applications) your enhancement suggestions is related to, create an issue on that repository and provide the following information:

* **Use a clear and descriptive title** for the issue to identify the suggestion.
* **Provide a step-by-step description of the suggested enhancement** in as many details as possible.
* **Provide specific examples to demonstrate the steps**. Include copy/pasteable snippets which you use in those examples, as [Markdown code blocks](https://help.github.com/en/articles/getting-started-with-writing-and-formatting-on-github).
* **Describe the current behavior** and **explain which behavior you expected to see instead** and why.
* **Include screenshots and animated GIFs** which show you following the described steps and clearly demonstrate the problem. You can try to **record the GIF of the steps to reproduce the bug**. You can use [licecap](https://www.cockos.com/licecap/) to record GIFs on macOS and Windows, and [silentcast](https://github.com/colinkeenan/silentcast) on Linux.
* **Explain why this enhancement would be useful** to most embARC OSP users and isn't something that already existed as similiar feature.
* **List some other text software platform where this enhancement exists.**
* **Specify the name and version of the OS you're using.**

#### [Template For Submitting Enhancement Suggestions](./ISSUE_TEMPLATE.md)

### Your First Code Contribution

Unsure where to begin contributing to embARC OSP? You can start by looking through the existing issues.

If you want to read about using embARC OSP or developing board support package, peripheral drivers, libraries, middlewares or applications in embARC OSP, the [embARC OSP documentation](https://embarc.org/embarc_osp) is free and available online.

### Pull Requests

* Include screenshots and/or animated GIFs in your pull request whenever possible.
* Follow the [C](#c-styleguide) styleguide.
* Include thoughtfully-worded, well-structured specs in your doc folder or `readme.md`.
* Document new code based on the [Documentation Styleguide](#documentation-styleguide)
* End files with a newline.

## Styleguides

### Git Commit Messages

* Following the [good commit guideline](https://chris.beams.io/posts/git-commit/).
* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
* Limit the first line to 72 characters or less
* Reference issues and pull requests liberally
* Consider starting the commit message with an category and colon:
    * `arc:` changes related to arc core
    * `board:` changes related to board support packages
    * `device:` changes related to device hal
    * `peripheral:` changes releated to peripheral drivers
    * `library:` changes related to libraries
    * `middleware:` changes related to middlewares
    * `os:` changes related to OSes
    * `test:` changes related to test cases
    * `doc:` changes related to documentation
    * `build:` changes related to build system
    * `tool:` changes related to tools
    * `example:` changes related to examples
    * `application:` changes related to applications
    * `misc:` changes not categorized

### C Styleguide

All C source code must adhere to [Linux Kernel Coding Style](https://www.kernel.org/doc/Documentation/CodingStyle).
Here we use [AStyle tool](http://astyle.sourceforge.net/) to format the source code. The astyle option is provided [here](https://github.com/foss-for-synopsys-dwc-arc-processors/embarc_osp/blob/master/.astyle/embarc_astylerc).

To help with writing c source code in editor, you can install plugin for your editor.
- For sublime-text editor, you can use [SublimeAStyleFormatter](https://packagecontrol.io/packages/SublimeAStyleFormatter) plugin.

### Documentation Styleguide

* Use [Doxygen](http://www.doxygen.nl).
    - To help with documentation in source code, you can install doxygen plugins for your editor.
        + For sublime-text editor, you can use [Doc​Blockr](https://packagecontrol.io/packages/DocBlockr) plugin.
* Use [Markdown](https://daringfireball.net/projects/markdown).

#### Example

```c
/**
 * \brief       Reserve a DMA channel, and bind it with dma_chn, and set the dma
 *              trigger source
 *
 * \param[in]   channel                This can be DMA_CHN_ANY or any valid
 *                                     channel id. For DMA_CHN_ANY, it will try
 *                                     to peek an available channel. For any
 *                                     valid channel id, it will try to reserve
 *                                     that channel.
 * \param       dma_chn                uDMA channel structure, should not be
 *                                     NULL
 * \param[in]   source                 DMA trigger source, this can be any value
 *                                     in dma_request_source_t enum
 *
 * \retval      DMA_CHN_INVALID        dma_chn is NULL, or channel is not a
 *                                     valid one, or there is no channel
 *                                     available now
 * \retval      0-DMA_ALL_CHANNEL_NUM  the channel id that reserved
 */
```

## Additional Notes

### Issue and Pull Request Labels

This section lists the labels we use to help us track and manage issues and pull requests. Most labels are used across all embARC repositories.

[GitHub search](https://help.github.com/en/articles/searching-issues-and-pull-requests) makes it easy to use labels for finding groups of issues or pull requests you're interested in.

The labels are loosely grouped by their purpose, but it's not required that every issue have a label from every group or that an issue can't have more than one label from the same group.

## Special Thanks to the Atom Project
<p>The contribution guideline takes a lot of reference from the <a class="reference external" href="https://github.com/atom/atom/blob/master/CONTRIBUTING">Atom Contribution Guideline</a>.</p>
