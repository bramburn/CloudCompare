import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Heading from '@theme/Heading';
import Layout from '@theme/Layout';
import HomepageFeatures from '@site/src/components/HomepageFeatures';
import Workflow from '@site/src/components/Workflow';
import PluginSet from '@site/src/components/PluginSet';
import useBaseUrl from '@docusaurus/useBaseUrl';

import styles from './index.module.css';

function HomepageHeader() {
  const {siteConfig} = useDocusaurusContext();
  const logoUrl = useBaseUrl('/img/logo.png');
  return (
    <header className={styles.heroBanner}>
      <div className={styles.heroBackdrop} aria-hidden="true">
        <div className={styles.heroGrid} />
        <div className={styles.heroGlowA} />
        <div className={styles.heroGlowB} />
      </div>
      <div className="container">
        <div className={styles.heroInner}>
          <div className={styles.heroText}>
            <div className={styles.heroBadge}>
              <span className={styles.heroBadgeDot} />
              v2.14 fork · Icelabz Surveying
            </div>
            <Heading as="h1" className={styles.heroTitle}>
              {siteConfig.title}
            </Heading>
            <p className={styles.heroSubtitle}>{siteConfig.tagline}</p>
            <p className={styles.heroLead}>
              Open-source 3D point cloud and triangular mesh processing. C++17,
              Qt 6, OpenGL 2.1+, CMake 3.10+. Maintained as the Icelabz
              surveying fork of the upstream{' '}
              <Link
                className={styles.heroInlineLink}
                to="https://github.com/CloudCompare/CloudCompare">
                CloudCompare
              </Link>
              .
            </p>
            <div className={styles.buttons}>
              <Link
                className="button button--primary button--lg"
                to="/docs/getting-started/overview">
                Get started →
              </Link>
              <Link
                className="button button--secondary button--lg"
                to="/docs/build/windows">
                Build on Windows
              </Link>
              <Link
                className="button button--outline button--lg"
                to="/docs/plugins/overview">
                Plugin guide
              </Link>
            </div>
            <div className={styles.heroStats}>
              <div className={styles.heroStat}>
                <span className={styles.heroStatValue}>16</span>
                <span className={styles.heroStatLabel}>self-contained plugins</span>
              </div>
              <div className={styles.heroStat}>
                <span className={styles.heroStatValue}>43</span>
                <span className={styles.heroStatLabel}>Rust unit tests in cc-rust</span>
              </div>
              <div className={styles.heroStat}>
                <span className={styles.heroStatValue}>230×</span>
                <span className={styles.heroStatLabel}>
                  kiddo KD-tree vs brute-force ICP (real .las)
                </span>
              </div>
              <div className={styles.heroStat}>
                <span className={styles.heroStatValue}>2</span>
                <span className={styles.heroStatLabel}>CI platforms (Windows + Linux); macOS community-supported</span>
              </div>
            </div>
          </div>
          <div className={styles.heroVisual} aria-hidden="true">
            <div className={styles.heroCard}>
              <div className={styles.heroCardBar}>
                <span className={`${styles.heroCardDot} ${styles.heroCardDotRed}`} />
                <span className={`${styles.heroCardDot} ${styles.heroCardDotYellow}`} />
                <span className={`${styles.heroCardDot} ${styles.heroCardDotGreen}`} />
                <span className={styles.heroCardTitle}>CloudCompare.exe</span>
              </div>
              <div className={styles.heroCardBody}>
                <div className={styles.heroCardLine}>
                  <span className={styles.heroCardPrompt}>$</span>
                  <span className={styles.heroCardCmd}>cc-configure.cmd</span>
                </div>
                <div className={styles.heroCardLine}>
                  <span className={styles.heroCardOutput}>-- The C compiler identification is MSVC 19.44</span>
                </div>
                <div className={styles.heroCardLine}>
                  <span className={styles.heroCardOutput}>-- Detecting CXX compiler ABI info</span>
                </div>
                <div className={styles.heroCardLine}>
                  <span className={styles.heroCardOutput}>-- Found Qt 6.8.3 at C:/dev/tools/Qt/6.8.3</span>
                </div>
                <div className={styles.heroCardLine}>
                  <span className={styles.heroCardOutput}>-- Found PkgConfig: 18 plugins</span>
                </div>
                <div className={styles.heroCardLine}>
                  <span className={styles.heroCardOutput}>-- Configuring done · 0.84s</span>
                </div>
                <div className={styles.heroCardLine}>
                  <span className={styles.heroCardPrompt}>$</span>
                  <span className={styles.heroCardCmd}>cc-build.cmd</span>
                </div>
                <div className={styles.heroCardLine}>
                  <span className={styles.heroCardOutput}>[100%] Built target CloudCompare.exe</span>
                </div>
                <div className={styles.heroCardLine}>
                  <span className={styles.heroCardSuccess}>✓ ready · 6m 12s</span>
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>
    </header>
  );
}

export default function Home() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <Layout
      title={`${siteConfig.title} — ${siteConfig.tagline}`}
      description="Documentation for the Icelabz fork of CloudCompare: build, plugins, architecture, and CI for 3D point cloud and mesh processing.">
      <HomepageHeader />
      <main>
        <Workflow />
        <HomepageFeatures />
        <PluginSet />
      </main>
    </Layout>
  );
}
