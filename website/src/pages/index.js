import Link from '@docusaurus/Link';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';
import Heading from '@theme/Heading';
import Layout from '@theme/Layout';
import HomepageFeatures from '@site/src/components/HomepageFeatures';

import styles from './index.module.css';

function HomepageHeader() {
  const {siteConfig} = useDocusaurusContext();
  return (
    <header className={styles.heroBanner}>
      <div className="container">
        <Heading as="h1" className={styles.heroTitle}>
          {siteConfig.title}
        </Heading>
        <p className={styles.heroSubtitle}>{siteConfig.tagline}</p>
        <p className={styles.heroLead}>
          Open-source 3D point cloud and triangular mesh processing. C++17, Qt 6,
          OpenGL 2.1+, CMake 3.10+. Maintained as the Icelabz surveying fork of
          the upstream{' '}
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
            Get started
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
        <HomepageFeatures />
      </main>
    </Layout>
  );
}
